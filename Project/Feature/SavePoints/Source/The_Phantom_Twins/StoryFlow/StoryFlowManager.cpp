// Fill out your copyright notice in the Description page of Project Settings.


#include "StoryFlowManager.h"
#include "StorySaveState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "The_Phantom_Twins/Chapter/IdentifyChracterData.h"
#include "The_Phantom_Twins/OutGame/SaveGame/TPTSaveGame.h"

DEFINE_LOG_CATEGORY(LogStoryFlow);

void UStoryFlowManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    //todo:: 서버에서 호출뒤, 클라에서 세팅이 되면서 서버에 OnPreLoadMap 함수를 요청함... 순서적으로 맵로드시에 데이터 클리어를 해야해서 이런 구조가 됨.
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UStoryFlowManager::OnPreLoadMap);
}

void UStoryFlowManager::Deinitialize()
{
    Super::Deinitialize();

    FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
}

void UStoryFlowManager::OnPreLoadMap(const FString& MapName)
{
    this->ClearAllData();
}

void UStoryFlowManager::RegisterData(FName DataName, UObject* Data)
{
    if (nullptr == Data)
    {
        UE_LOG(LogStoryFlow, Error, TEXT("RegisterData: Data is invalid or nullptr"));
        return;
    }
    DataMap.Add(DataName, Data);
}

void UStoryFlowManager::UnregisterData(FName DataName)
{
    DataMap.Remove(DataName);
    Subscribers.Remove(DataName);

    // Remove any matching delegates from BPDelegateMap
    TArray<FGuid> ToRemove;
    for (const auto& Pair : BPDelegateMap)
    {
        const FGuid& ID = Pair.Key;
        if (Subscribers.Contains(DataName))
        {
            const auto& List = Subscribers[DataName];
            if (List.ContainsByPredicate([&](const TPair<FGuid, FCallback>& SubPair) { return SubPair.Key == ID; }))
            {
                ToRemove.Add(ID);
            }
        }
    }
    for (const FGuid& ID : ToRemove)
    {
        BPDelegateMap.Remove(ID);
    }

    CoreDataSet.Remove(DataName);
}

void UStoryFlowManager::SetData(FName DataName, UObject* NewValue)
{
    if (nullptr != NewValue && DataMap.Contains(DataName))
    {
        DataMap[DataName] = NewValue;
        NotifySubscribers(DataName);
    }
    UE_LOG(LogStoryFlow, Warning, TEXT("SetData: Data is invalid or Unregister"));
}

UObject* UStoryFlowManager::GetData(FName DataName) const
{
    if (const TObjectPtr<UObject>* Found = DataMap.Find(DataName))
    {
        return *Found;
    }
    UE_LOG(LogStoryFlow, Warning, TEXT("GetData: Data is Unregister"));

    return nullptr;
}

FGuid UStoryFlowManager::Subscribe(FName DataName, FCallback Callback)
{
    FGuid ID = FGuid::NewGuid();
    Subscribers.FindOrAdd(DataName).Add(TPair<FGuid, FCallback>(ID, Callback));
    return ID;
}

UStoryFlowManager::FCallbackID UStoryFlowManager::SubscribeBP(FName DataName, const FOnDataChangedBP& Callback)
{
    FGuid ID = FGuid::NewGuid();

    Subscribers.FindOrAdd(DataName).Add(TPair<FGuid, FCallback>(ID, [this, Callback, ID](FName Name, const UObject* Obj)
        {
            if (DeferredRemovals.Contains(ID)) return;
            Callback.ExecuteIfBound(Name, const_cast<UObject*>(Obj));
        }));

    BPDelegateMap.Add(ID, Callback);
    return ID;
}

void UStoryFlowManager::Unsubscribe(FName DataName, FGuid CallbackId)
{
    DeferredRemovals.Add(CallbackId);
    if (auto* List = Subscribers.Find(DataName))
    {
        List->RemoveAll([&](const TPair<FGuid, FCallback>& Pair)
            {
                return Pair.Key == CallbackId;
            });
    }
    BPDelegateMap.Remove(CallbackId);
}

void UStoryFlowManager::ClearAllData(bool bRemoveCoreData)
{
    TArray<FName> KeysToRemove;
    for (const auto& Pair : DataMap)
    {
        if (bRemoveCoreData || !CoreDataSet.Contains(Pair.Key))
        {
            KeysToRemove.Add(Pair.Key);
        }
    }

    for (const FName& Key : KeysToRemove)
    {
        UnregisterData(Key);
    }
}

void UStoryFlowManager::MarkAsCoreData(FName DataName, bool bCoreData)
{
    if(bCoreData)
        CoreDataSet.Add(DataName);
    else
        CoreDataSet.Remove(DataName);
}

void UStoryFlowManager::NotifySubscribers(FName DataName)
{
    if (const auto* Value = DataMap.Find(DataName))
    {
        if (auto* List = Subscribers.Find(DataName))
        {
            TArray<TPair<FGuid, FCallback>> Copy = *List; // 복사본으로 순회
            for (const auto& [ID, Callback] : Copy)
            {
                if (!DeferredRemovals.Contains(ID) && Callback)
                {
                    Callback(DataName, *Value);
                }
            }
            DeferredRemovals.Empty();
        }
    }
}

void UStoryFlowManager::SaveToDisk()
{
    UStorySaveState* SaveGame = Cast<UStorySaveState>(UGameplayStatics::CreateSaveGameObject(UStorySaveState::StaticClass()));
    for (const auto& Pair : DataMap)
    {
        if (Pair.Value)
        {
            SaveGame->SavedData.Add(Pair.Key, Pair.Value->GetName());
        }
    }
    UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("StorySlot"), 0);
}

void UStoryFlowManager::LoadFromDisk()
{
    if (UStorySaveState* SaveGame = Cast<UStorySaveState>(UGameplayStatics::LoadGameFromSlot(TEXT("StorySlot"), 0)))
    {
        for (const auto& Pair : SaveGame->SavedData)
        {
            UObject* Found = StaticFindObject(UObject::StaticClass(), ANY_PACKAGE, *Pair.Value);
            if (Found)
            {
                DataMap.Add(Pair.Key, Found);
            }
        }
    }
}

void UStoryFlowManager::SaveGameData(const FString& SlotName)
{
    // 1. SaveGame 객체를 생성합니다.
    UTPTSaveGame* SaveGameObject = Cast<UTPTSaveGame>(UGameplayStatics::CreateSaveGameObject(UTPTSaveGame::StaticClass()));
    if (!SaveGameObject)
    {
        UE_LOG(LogStoryFlow, Error, TEXT("Failed to create SaveGameObject."));
        return;
    }

    // 2. 캐릭터 선택 정보를 가져와서 SaveGameObject에 직접 저장합니다.
    UIdentifyChracterData* SelectedCharacterData = GetDataAs<UIdentifyChracterData>(FName("IdentifyChractor"));
    if (SelectedCharacterData)
    {
        SaveGameObject->HostSelectedCharacter = SelectedCharacterData->Host;
        SaveGameObject->ClientSelectedCharacter = SelectedCharacterData->Client;
    }
    else
    {
        UE_LOG(LogStoryFlow, Warning, TEXT("Character select data not found. Saving without it."));
    }

    // 3. 각 플레이어의 ID, 위치, 회전 정보만 FPlayerSaveData에 저장합니다.
    UWorld* World = GetWorld();
    if (!World || !World->GetGameState()) return;

    for (const APlayerState* PS : World->GetGameState()->PlayerArray)
    {
        if (PS)
        {
            FPlayerSaveData PlayerData;

            // 고유 ID 저장
            const FUniqueNetIdRepl& UniqueId = PS->GetUniqueId();
            if (UniqueId.IsValid())
            {
                PlayerData.PlayerID = UniqueId.ToString();
            }
            else
            {
                PlayerData.PlayerID = PS->GetPlayerName(); // 대체 수단
            }

            // 위치 및 회전값 저장
            APawn* Pawn = PS->GetPawn();
            if (Pawn)
            {
                PlayerData.PlayerLocation = Pawn->GetActorLocation();
                PlayerData.PlayerRotation = Pawn->GetActorRotation();
            }

            // *** 중요: 이 루프 안에서는 더 이상 캐릭터 선택 정보를 저장하지 않습니다! ***
            
            SaveGameObject->Players.Add(PlayerData);
        }
    }

    // 4. 데이터를 슬롯에 저장합니다.
    UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
    UE_LOG(LogStoryFlow, Log, TEXT("Game data saved to slot: %s"), *SlotName);
}

void UStoryFlowManager::LoadGameData(const FString& SlotName)
{
    UTPTSaveGame* LoadedGame = Cast<UTPTSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!LoadedGame)
    {
        UE_LOG(LogStoryFlow, Warning, TEXT("Save file not found in slot: %s."), *SlotName);
        return;
    }

    // 새 데이터 객체를 만들고, 저장된 값으로 채웁니다.
    UIdentifyChracterData* CharData = NewObject<UIdentifyChracterData>(this);
    CharData->Host = LoadedGame->HostSelectedCharacter;
    CharData->Client = LoadedGame->ClientSelectedCharacter;

    // 매니저에 등록
    RegisterData(FName("CharacterSelect"), CharData);
    SetData(FName("CharacterSelect"), CharData);

    // 참고: 플레이어 위치 등은 GameMode에서 LoadedGame->Players 배열을 직접 참조하여 사용 가능
    UE_LOG(LogStoryFlow, Log, TEXT("Character selection data loaded successfully."));
}
