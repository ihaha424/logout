// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTSaveGameManager.h"

#include "AbilitySystemComponent.h"
#include "TPTLocalPlayerSaveGame.h"
#include "TPTSaveGame.h"
#include "TPTSaveGameHelperLibrary.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/BoxObject.h"
#include "Objects/Door.h"
#include "Objects/InteractHideObject.h"
#include "Objects/InventoryComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "SzObjects/ItemObject.h"

void UTPTSaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeSaveTargets();
}
void UTPTSaveGameManager::Deinitialize()
{
    PlayerSaveGames.Empty();
    GameSaveGame = nullptr;
    Super::Deinitialize();
}

void UTPTSaveGameManager::SaveUpdate()
{
    GameSaveGame->DataFragmentNum++;
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTSaveGame>(GameSaveGame);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[0]);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[1]);
}

void UTPTSaveGameManager::InitializeSaveTargets()
{
    GameSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>());
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>());

	PlayerSaveGames[0]->CoreEnergy = 5;
	PlayerSaveGames[1]->CoreEnergy = 5;
	PlayerSaveGames[0]->InventorySlots.Empty();
	PlayerSaveGames[1]->InventorySlots.Empty();

    DoorActors.Empty();
    ItemActors.Empty();
    HideObjectActors.Empty();
    ItemBoxActors.Empty();
    AIActors.Empty();

    TArray<AActor*> TempActors;

    // 문 액터
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            ADoor* Door = Cast<ADoor>(Actor);
            FGuid DoorID = Door->PersistentActorID;
            FDoorState DoorState;
            DoorState.bIsOpened = Door->bIsActived;
            DoorState.bIsExist = true;
			DoorActors.Add(DoorID, Actor);
            GameSaveGame->DoorStates.Add(DoorID, DoorState);
        }
    }

    // 아이템 액터
    TempActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemObject::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
			AItemObject* Item = Cast<AItemObject>(Actor);
            FGuid ItemID = Item->PersistentActorID;
            ItemActors.Add(ItemID, Actor);
            GameSaveGame->ItemStates.Add(ItemID, true);
        }
    }

    // 숨는 오브젝트 액터
    TempActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractHideObject::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            AInteractHideObject* HideObject = Cast<AInteractHideObject>(Actor);
            FGuid HideObjectID = HideObject->PersistentActorID;
            HideObjectActors.Add(HideObjectID, Actor);
            GameSaveGame->HideObjectStates.Add(HideObjectID, true);
        }
    }

    // 아이템 박스 액터
    TempActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoxObject::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            AItemObject* ItemBox = Cast<AItemObject>(Actor);
            FGuid BoxID = ItemBox->PersistentActorID;
            ItemBoxActors.Add(BoxID, Actor);
            GameSaveGame->ItemBoxStates.Add(BoxID, ItemBox->bIsActived);
        }
    }

    // AI
    TempActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIBaseCharacter::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(Actor);
            FGuid AIID = AI->PersistentActorID;
            ItemBoxActors.Add(AIID, Actor);
            GameSaveGame->AIStates.Add(AIID, true);
        }
    }
}

void UTPTSaveGameManager::TempSaveByID(const FGuid& ObjectID, const bool bIsExist)
{
    if (!GameSaveGame)
        return;

    // 문 액터에서 매칭되는 ObjectID가 있으면 상태 업데이트
    if (AActor** DoorActor = DoorActors.Find(ObjectID))
    {
        ADoor* Door = Cast<ADoor>(*DoorActor);
        if (Door)
        {
            FDoorState DoorState;
            DoorState.bIsOpened = Door->bIsActived;
            DoorState.bIsExist = bIsExist;
            GameSaveGame->DoorStates.Add(ObjectID, DoorState);
            return;
        }
    }

    // 아이템 액터 상태 업데이트
    if (AActor** ItemActor = ItemActors.Find(ObjectID))
    {
        AItemObject* Item = Cast<AItemObject>(*ItemActor);
        if (Item)
        {
            GameSaveGame->ItemStates.Add(ObjectID, bIsExist);
            return;
        }
    }

    // 숨는 오브젝트 상태 업데이트
    if (AActor** HideActor = HideObjectActors.Find(ObjectID))
    {
        AInteractHideObject* HideObj = Cast<AInteractHideObject>(*HideActor);
        if (HideObj)
        {
            GameSaveGame->HideObjectStates.Add(ObjectID, bIsExist);
            return;
        }
    }

    // 아이템 박스 상태 업데이트
    if (AActor** ItemBoxActor = ItemBoxActors.Find(ObjectID))
    {
        ABoxObject* Box = Cast<ABoxObject>(*ItemBoxActor);
        if (Box)
        {
            bool bIsOpened = Box->bIsActived;
            GameSaveGame->ItemBoxStates.Add(ObjectID, bIsOpened);
            return;
        }
    }

    // AI 상태 업데이트
    if (AActor** AIActor = AIActors.Find(ObjectID))
    {
        AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*AIActor);
        if (AI)
        {
            GameSaveGame->AIStates.Add(ObjectID, bIsExist);
            return;
        }
    }
}

void UTPTSaveGameManager::TempSavePlayer(const APlayerController* PC, const bool bIsHost)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
    const UPlayerAttributeSet* AttributeSet = Player->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
	int32 CoreEnergy = AttributeSet->GetCoreEnergy();
    APS_Player* PlayerPS = Cast<APS_Player>(Player->GetPlayerState());
    TArray<FItemSlot> InventorySlots = PlayerPS->InventoryComp->InventorySlots;

    if (bIsHost)
    {
	    PlayerSaveGames[0]->CoreEnergy = CoreEnergy;
    	PlayerSaveGames[0]->InventorySlots = InventorySlots;
    }
	else
	{
		PlayerSaveGames[1]->CoreEnergy = CoreEnergy;
		PlayerSaveGames[1]->InventorySlots = InventorySlots;
	}
}

void UTPTSaveGameManager::SaveRestartPoint(const FVector& PlayerLocation, const FRotator& PlayerRotation)
{
	GameSaveGame->PlayerLocation = PlayerLocation;
	GameSaveGame->PlayerRotation = PlayerRotation;
}
void UTPTSaveGameManager::LoadSaveGame()
{
    // 전체 게임 세이브 데이터 불러오기
    GameSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();

    // 플레이어별 세이브 데이터 불러오기 (호스트: 0, 클라이언트:1)
    if (PlayerSaveGames.Num() < 2)
        PlayerSaveGames.SetNum(2);

    PlayerSaveGames[0] = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>(TEXT("LocalPlayer0Slot"), 0);
    PlayerSaveGames[1] = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>(TEXT("LocalPlayer1Slot"), 1);

    UE_LOG(LogTemp, Log, TEXT("SaveGameManager: Loaded SaveGame and PlayerSaveGames."));
}

void UTPTSaveGameManager::ApplySaveData()
{
    if (!GameSaveGame)
        return;

    // 문 상태 적용
    for (const auto& DoorPair : GameSaveGame->DoorStates)
    {
        FGuid DoorID = DoorPair.Key;
        FDoorState DoorState = DoorPair.Value;

        if (AActor** ActorPtr = DoorActors.Find(DoorID))
        {
            ADoor* Door = Cast<ADoor>(*ActorPtr);
            if (!DoorPair.Value.bIsExist)
            {
				Door->Destroyed();
            }
            // 여기 열어줘야함
        	//Door->SetActived(DoorState.bIsOpened);
        }
    }

    // 아이템 상태 적용
    for (const auto& ItemPair : GameSaveGame->ItemStates)
    {
        FGuid ItemID = ItemPair.Key;
        bool bIsExist = ItemPair.Value;

        if (AActor** ActorPtr = ItemActors.Find(ItemID))
        {
            AItemObject* Item = Cast<AItemObject>(*ActorPtr);
            if (!bIsExist)
            {
                Item->Destroyed();
            }
        }
    }

    // 숨는 오브젝트 상태 적용
    for (const auto& HidePair : GameSaveGame->HideObjectStates)
    {
        FGuid HideID = HidePair.Key;
        bool bIsExist = HidePair.Value;

        if (AActor** ActorPtr = HideObjectActors.Find(HideID))
        {
            AInteractHideObject* HideObj = Cast<AInteractHideObject>(*ActorPtr);
            if (!bIsExist)
            {
                HideObj->Destroyed();
            }
        }
    }

    // 아이템 박스 상태 적용
    for (const auto& BoxPair : GameSaveGame->ItemBoxStates)
    {
        FGuid BoxID = BoxPair.Key;
        bool bIsOpened = BoxPair.Value;

        if (AActor** ActorPtr = ItemBoxActors.Find(BoxID))
        {
            ABoxObject* Box = Cast<ABoxObject>(*ActorPtr);
            if (Box)
            {
            	// 여기 열어줘야함
                //Box->SetActived(bIsOpened);
            }
        }
    }

    // AI 상태 적용
    for (const auto& AIPair : GameSaveGame->AIStates)
    {
        FGuid AIID = AIPair.Key;
        bool bIsExist = AIPair.Value;

        if (AActor** ActorPtr = AIActors.Find(AIID))
        {
            AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*ActorPtr);
            if (!bIsExist)
            {
                AI->Destroyed();
            }
        }
    }
}