// Fill out your copyright notice in the Description page of Project Settings.


#include "StoryFlowManager.h"
#include "StorySaveState.h"

DEFINE_LOG_CATEGORY(LogStoryFlow);

void UStoryFlowManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FCoreUObjectDelegates::PostLoadMapWithWorld.AddStatic(&UStoryFlowManager::OnPostLoadMap);
}

void UStoryFlowManager::Deinitialize()
{
    Super::Deinitialize();

    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UStoryFlowManager::OnPostLoadMap(UWorld* LoadedWorld)
{
    if (LoadedWorld && LoadedWorld->IsGameWorld())
    {
        if (UGameInstance* GI = LoadedWorld->GetGameInstance())
        {
            if (UStoryFlowManager* Subsystem = GI->GetSubsystem<UStoryFlowManager>())
            {
                Subsystem->ClearAllData();
            }
        }
    }
}

void UStoryFlowManager::RegisterData(FName DataName, UObject* Data)
{
    if (nullptr == Data)
    {
        UE_LOG(LogStoryFlow, Error, TEXT("Data is invalid or nullptr"));
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
}

UObject* UStoryFlowManager::GetData(FName DataName) const
{
    if (const TObjectPtr<UObject>* Found = DataMap.Find(DataName))
    {
        return *Found;
    }
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
