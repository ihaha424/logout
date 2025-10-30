// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTSaveGameManager.h"

#include "AbilitySystemComponent.h"
#include "SaveIDComponent.h"
#include "TPTLocalPlayerSaveGame.h"
#include "TPTSaveGame.h"
#include "TPTSaveGameHelperLibrary.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Character/AIScanner.h"
#include "Attribute/PlayerAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "Math/UnitConversion.h"
#include "Objects/BoxObject.h"
#include "Objects/DataFragment.h"
#include "Objects/Door.h"
#include "Objects/InteractHideObject.h"
#include "Objects/InventoryComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "SzObjects/ItemObject.h"
#include "Tags/TPTGameplayTags.h"

UTPTSaveGameManager::UTPTSaveGameManager()
{
    GameSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>("MainSlot",  0, true);
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>("MainSlot", 0, true));
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>("MainSlot",  0, true));
    static ConstructorHelpers::FClassFinder<UGameplayEffect> EffectClass(TEXT("/Game/ThePhantomTwins/Characters/Blueprints/GE/SetPlayerAttribute/BPGE_CoreEnergySet.BPGE_CoreEnergySet_C"));
    if (EffectClass.Succeeded())
    {
        CoreEnergyEffect = EffectClass.Class;
    }

    bActorsInitialized = false;
    bPlayerInitialized = false;
}

void UTPTSaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    //FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::InitializeSaveTargets);
}
void UTPTSaveGameManager::Deinitialize()
{
    PlayerSaveGames.Empty();
    GameSaveGame = nullptr;
    Super::Deinitialize();
}

void UTPTSaveGameManager::SaveUpdate()
{
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTSaveGame>(GameSaveGame);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[0]);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[1]);
}

void UTPTSaveGameManager::BeginWorldRestoreSequence(UWorld* World)
{
    CurrentWorld = World;

    InitializeSaveTargets();

    ApplyActorSaveGame();

    if (CurrentWorld)
    {
        CurrentWorld->AddOnActorSpawnedHandler(
            FOnActorSpawned::FDelegate::CreateUObject(this, &UTPTSaveGameManager::OnActorSpawned));
    }
}

void UTPTSaveGameManager::InitializeSaveTargets()
{
    TArray<AActor*> TempActors;
    // 문 액터
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            ADoor* Door = Cast<ADoor>(Actor);
        	/*USaveIDComponent* SaveIDComp = Door->FindComponentByClass<USaveIDComponent>();
            if (!IsValid(SaveIDComp))
            {
                continue;
            }*/
            FName DoorID = Door->GetFName();

            if (DoorActorsMap.Contains(DoorID))
            {
                DoorActorsMap[DoorID] = Door;
            }
 
            if (!bActorsInitialized)
            {
                DoorActorsMap.Add(DoorID, Door);
            	FDoorState DoorState;
				DoorState.bIsOpen = Door->bIsActived;
				DoorState.bIsUnLocked = Door->bIsAllTriggered;
				DoorState.bIsExist = true;
                GameSaveGame->DoorStates.FindOrAdd(DoorID, DoorState);
            }
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
            //USaveIDComponent* SaveIDComp = Item->FindComponentByClass<USaveIDComponent>();
            //if (!IsValid(SaveIDComp))
            //{
            //    continue;
            //}
            FName ItemID = Actor->GetFName();
            if (Cast < ADataFragment>(Item))
            {
	            TPT_LOG(SaveGameLog,Warning,TEXT("DataFragment : %s"), *ItemID.ToString())
            }
            if (ItemActorsMap.Contains(ItemID))
            {
                ItemActorsMap[ItemID] = Item;
            }

            if (!bActorsInitialized)
            {
                ItemActorsMap.Add(ItemID, Item);
                GameSaveGame->ItemStates.FindOrAdd(ItemID, true);
            }
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
            //USaveIDComponent* SaveIDComp = HideObject->FindComponentByClass<USaveIDComponent>();
            //if (!IsValid(SaveIDComp))
            //{
            //    continue;
            //}
            FName HideObjectID = Actor->GetFName();

            if (HideObjectActorsMap.Contains(HideObjectID))
            {
                HideObjectActorsMap[HideObjectID] = HideObject;
            }

            if (!bActorsInitialized)
	        {
                HideObjectActorsMap.Add(HideObjectID, HideObject);
		        GameSaveGame->HideObjectStates.FindOrAdd(HideObjectID, true);
	        }
        }
    }

    // 아이템 박스 액터
    TempActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoxObject::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            ABoxObject* Box = Cast<ABoxObject>(Actor);
            //USaveIDComponent* SaveIDComp = Box->FindComponentByClass<USaveIDComponent>();
            //if (!IsValid(SaveIDComp))
            //{
            //    continue;
            //}
            FName BoxID = Actor->GetFName();

            if (ItemBoxActorsMap.Contains(BoxID))
            {
                ItemBoxActorsMap[BoxID] = Box;
            }

            if (!bActorsInitialized)
	        {
                ItemBoxActorsMap.Add(BoxID, Box);
		        GameSaveGame->ItemBoxStates.FindOrAdd(BoxID, Box->bIsActived);
	        }
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
            //USaveIDComponent* SaveIDComp = AI->FindComponentByClass<USaveIDComponent>();
            //if (!IsValid(SaveIDComp))
            //{
            //    continue;
            //}
            FName AIID = Actor->GetFName();

            if (AIActorsMap.Contains(AIID))
            {
                AIActorsMap[AIID] = AI;
            }

            if (!bActorsInitialized)
            {
                AIActorsMap.Add(AIID, AI);
	            GameSaveGame->AIStates.FindOrAdd(AIID, true);
            }
        }
    }
    bActorsInitialized = true;
}

void UTPTSaveGameManager::OnActorSpawned(AActor* Spawned)
{
    if (!IsValid(Spawned)) return;
    if (UWorld* W = Spawned->GetWorld())
    {
        W->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(
            this, &UTPTSaveGameManager::RegisterReadyTarget, Spawned));
    }
}

void UTPTSaveGameManager::RegisterReadyTarget(AActor* Spawned)
{
    if (ADoor* Door = Cast<ADoor>(Spawned))
    {
        //USaveIDComponent* SaveIDComp = Door->FindComponentByClass<USaveIDComponent>();
        FName DoorID = Door->GetFName();

        if (FDoorState* State = GameSaveGame->DoorStates.Find(DoorID))
        {
            if (!State->bIsExist)
            {
                Door->Destroy();
                return;
            }
            Door->bIsAllTriggered = State->bIsUnLocked;
            Door->bIsActived = State->bIsOpen;
        }
        else
        {
            DoorActorsMap.Add(DoorID, Door);
            FDoorState DoorState;
            DoorState.bIsUnLocked = Door->bIsAllTriggered;
            DoorState.bIsOpen = Door->bIsActived;
            DoorState.bIsExist = true;
            GameSaveGame->DoorStates.FindOrAdd(DoorID, DoorState);
        }
        return;
    }

    // 아이템 액터
    if(AItemObject* Item = Cast<AItemObject>(Spawned))
    {
	    //USaveIDComponent* SaveIDComp = Item->FindComponentByClass<USaveIDComponent>();
        FName ItemID = Item->GetFName();

        if (const bool* bIsExist = GameSaveGame->ItemStates.Find(ItemID))
        {
            if (!*bIsExist)
            {
                Item->DestroyItem();
            }
        }
        else
    	{
    		ItemActorsMap.Add(ItemID, Item);
    		GameSaveGame->ItemStates.FindOrAdd(ItemID, true);
    	}
        return;
    }
 

    // 숨는 오브젝트 액터
    if (AInteractHideObject* HideObject = Cast<AInteractHideObject>(Spawned))
    {
	    //USaveIDComponent* SaveIDComp = HideObject->FindComponentByClass<USaveIDComponent>();
        FName HideObjectID = HideObject->GetFName();

        if (const bool* bIsExist = GameSaveGame->HideObjectStates.Find(HideObjectID))
        {
            if (!*bIsExist)
            {
                HideObject->Destroy();
            }
        }
        else
    	{
    		HideObjectActorsMap.Add(HideObjectID, HideObject);
    		GameSaveGame->HideObjectStates.FindOrAdd(HideObjectID, true);
    	}
        return;
    }
 

    // 아이템 박스 액터
    if(ABoxObject* Box = Cast<ABoxObject>(Spawned))
    {
	    //USaveIDComponent* SaveIDComp = Box->FindComponentByClass<USaveIDComponent>();
        FName BoxID = Box->GetFName();

        if (const bool *bIsOpened = GameSaveGame->ItemBoxStates.Find(BoxID))
        {
            Box->SetActive(*bIsOpened);
            Box->InitBoxOpen(*bIsOpened);
        }
        else
        {
            ItemBoxActorsMap.Add(BoxID, Box);
            GameSaveGame->ItemBoxStates.FindOrAdd(BoxID, Box->bIsActived);
        }
        return;
    }
    

    // AI
   if(AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(Spawned))
    {
	    //USaveIDComponent* SaveIDComp = AI->FindComponentByClass<USaveIDComponent>();
        FName AIID = AI->GetFName();

        if (const bool* bIsExist = GameSaveGame->AIStates.Find(AIID))
        {
            if (!*bIsExist)
            {
                AI->Destroy();
            }
        }
        else
    	{
    		AIActorsMap.Add(AIID, AI);
    		GameSaveGame->AIStates.FindOrAdd(AIID, true);
    	}
    }
}

void UTPTSaveGameManager::TempSaveByID(const FName& ObjectID, const bool bIsExist)
{
    if (!GameSaveGame)
        return;
    TPT_LOG(SaveGameLog, Warning, TEXT("22DataFragment : %s"), *GetFName().ToString())

    // 문 액터에서 매칭되는 ObjectID가 있으면 상태 업데이트
    if (AActor** DoorActor = DoorActorsMap.Find(ObjectID))
    {
        ADoor* Door = Cast<ADoor>(*DoorActor);
        if (Door)
        {
            FDoorState DoorState;
            DoorState.bIsUnLocked = Door->bIsAllTriggered;
            DoorState.bIsOpen = Door->bIsActived;
            DoorState.bIsExist = bIsExist;
            GameSaveGame->DoorStates[ObjectID] = DoorState;
            return;
        }
    }
    TPT_LOG(SaveGameLog, Warning, TEXT("33DataFragment : %s"), *GetFName().ToString())

    // 아이템 액터 상태 업데이트
    if (AActor** ItemActor = ItemActorsMap.Find(ObjectID))
    {
        TPT_LOG(SaveGameLog, Warning, TEXT("66DataFragment : %s"), *GetFName().ToString())

        AItemObject* Item = Cast<AItemObject>(*ItemActor);
        if (Cast<ADataFragment>(Item))
        {
            TPT_LOG(SaveGameLog, Warning, TEXT("44DataFragment : %s"), *ObjectID.ToString())
        }
        if (Item)
        {
            TPT_LOG(SaveGameLog, Warning, TEXT("55DataFragment : %s"), *ObjectID.ToString())
            GameSaveGame->ItemStates[ObjectID] = bIsExist;
            return;
        }
    }

    // 숨는 오브젝트 상태 업데이트
    if (AActor** HideActor = HideObjectActorsMap.Find(ObjectID))
    {
        AInteractHideObject* HideObj = Cast<AInteractHideObject>(*HideActor);
        if (HideObj)
        {
            GameSaveGame->HideObjectStates[ObjectID] = bIsExist;
            return;
        }
    }

    // 아이템 박스 상태 업데이트
    if (AActor** ItemBoxActor = ItemBoxActorsMap.Find(ObjectID))
    {
        ABoxObject* Box = Cast<ABoxObject>(*ItemBoxActor);
        if (Box)
        {
            bool bIsOpened = Box->bIsActived;
            GameSaveGame->ItemBoxStates[ObjectID] = bIsOpened;
            return;
        }
    }

    // AI 상태 업데이트
    if (AActor** AIActor = AIActorsMap.Find(ObjectID))
    {
        AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*AIActor);
        if (AI)
        {
            GameSaveGame->AIStates[ObjectID] = bIsExist;
            return;
        }
    }
}

void UTPTSaveGameManager::TempSavePlayer(const APlayerController* PC)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
    const UPlayerAttributeSet* AttributeSet = Player->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
	int32 CoreEnergy = AttributeSet->GetCoreEnergy();
    APS_Player* PlayerPS = Cast<APS_Player>(Player->GetPlayerState());
    TArray<FItemSlot> InventorySlots = PlayerPS->InventoryComp->InventorySlots;

    if (PC->IsLocalController())
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

void UTPTSaveGameManager::SetRestartPoint(const ADataFragment* DataFragment)
{
    if (!DataFragment)
    {
        TPT_LOG(SaveGameLog, Error, TEXT("no DataFragment"));
            return;
    }

    GameSaveGame->HostPlayerStart = DataFragment->HostPlayerLocation->GetComponentTransform();
    GameSaveGame->ClientPlayerStart = DataFragment->ClientPlayerLocation->GetComponentTransform();
}

FTransform UTPTSaveGameManager::GetRestartPoint(const bool bIsHost)
{
    if (bIsHost)
    {
        return GameSaveGame->HostPlayerStart;
    }
    return GameSaveGame->ClientPlayerStart;
}

void UTPTSaveGameManager::ApplyActorSaveGame()
{
    if (!GameSaveGame)
        return;

    // 문 상태 적용
    for (const auto& DoorPair : GameSaveGame->DoorStates)
    {
        FName DoorID = DoorPair.Key;
        FDoorState DoorState = DoorPair.Value;

        if (AActor** ActorPtr = DoorActorsMap.Find(DoorID))
        {
            ADoor* Door = Cast<ADoor>(*ActorPtr);
            if (!IsValid(Door))
            {
                continue;
            }
            if (!DoorPair.Value.bIsExist)
            {
				Door->Destroy();
            }
            Door->bIsAllTriggered = DoorState.bIsUnLocked;
            Door->bIsActived = DoorState.bIsOpen;
            Door->OnRep_bIsActived();
        }
    }

    // 아이템 상태 적용
    for (const auto& ItemPair : GameSaveGame->ItemStates)
    {
        FName ItemID = ItemPair.Key;
        bool bIsExist = ItemPair.Value;

        if (AActor** ActorPtr = ItemActorsMap.Find(ItemID))
        {
            AItemObject* Item = Cast<AItemObject>(*ActorPtr);
            if (!IsValid(Item))
            {
                continue;
            }
            if (!bIsExist)
            {
                Item->DestroyItem();
            }
        }
    }

    // 숨는 오브젝트 상태 적용
    for (const auto& HidePair : GameSaveGame->HideObjectStates)
    {
        FName HideID = HidePair.Key;
        bool bIsExist = HidePair.Value;

        if (AActor** ActorPtr = HideObjectActorsMap.Find(HideID))
        {
            AInteractHideObject* HideObj = Cast<AInteractHideObject>(*ActorPtr);
            if (!IsValid(HideObj))
            {
                continue;
            }
            if (!bIsExist)
            {
                HideObj->Destroy();
            }
        }
    }

    // 아이템 박스 상태 적용
    for (const auto& BoxPair : GameSaveGame->ItemBoxStates)
    {
        FName BoxID = BoxPair.Key;
        bool bIsOpened = BoxPair.Value;

        if (AActor** ActorPtr = ItemBoxActorsMap.Find(BoxID))
        {
            ABoxObject* Box = Cast<ABoxObject>(*ActorPtr);
            if (!IsValid(Box))
            {
                continue;
            }
            if (Box)
            {
                Box->SetActive(bIsOpened);
                Box->InitBoxOpen(bIsOpened);
            }
        }
    }

    // AI 상태 적용
    for (const auto& AIPair : GameSaveGame->AIStates)
    {
        FName AIID = AIPair.Key;
        bool bIsExist = AIPair.Value;

        if (AActor** ActorPtr = AIActorsMap.Find(AIID))
        {
            AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*ActorPtr);
            if (!IsValid(AI))
            {
                continue;
            }
            if (!bIsExist)
            {
                AI->Destroy();
            }
        }
    }
}
void UTPTSaveGameManager::InitializeSavePlayer()
{
    if (bPlayerInitialized)
    {
        return;
    }
    PlayerSaveGames[0]->CoreEnergy = 5;
    PlayerSaveGames[1]->CoreEnergy = 5;

    PlayerSaveGames[0]->InventorySlots.Init(FItemSlot(), 5);
    PlayerSaveGames[1]->InventorySlots.Init(FItemSlot(), 5);

    bPlayerInitialized = true;
}

void UTPTSaveGameManager::ApplyAuthorityPlayerSaveGame(APlayerController* PC)
{
	if(!PC->HasAuthority())
    {
        return;
    }

	APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
    NULLCHECK_RETURN_LOG(Player, SaveGameLog, Error, );

    APS_Player* PlayerPS = Cast<APS_Player>(Player->GetPlayerState());
    NULLCHECK_RETURN_LOG(PlayerPS, SaveGameLog, Error, );
    UInventoryComponent* Inventory = PlayerPS->InventoryComp;

    UAbilitySystemComponent* ASC = PlayerPS->GetAbilitySystemComponent();
    NULLCHECK_RETURN_LOG(ASC, SaveGameLog, Error, );

    if (PC->IsLocalController())
    {
        ApplyPlayerEffect(ASC,PlayerSaveGames[0]->CoreEnergy);

        const int32 MaxSlots = 5;
        for (int32 SlotIdx = 0; SlotIdx < MaxSlots; ++SlotIdx)
        {
            FItemSlot Slot = PlayerSaveGames[0]->InventorySlots[SlotIdx];
            if (Slot.ItemType == EItemType::None || Slot.ItemQuantity <= 0)
            {
                continue;
            }

            for (int32 n = 0; n < Slot.ItemQuantity; ++n)
            {
                Inventory->AddItem(Slot.ItemType);
            }
        }
    }
    else
    {
        ApplyPlayerEffect(ASC, PlayerSaveGames[1]->CoreEnergy);
        const int32 MaxSlots = 5;
        for (int32 SlotIdx = 0; SlotIdx < MaxSlots; ++SlotIdx)
        {
            FItemSlot Slot = PlayerSaveGames[1]->InventorySlots[SlotIdx];
            if (Slot.ItemType == EItemType::None || Slot.ItemQuantity <= 0)
            {
                continue;
            }

            for (int32 n = 0; n < Slot.ItemQuantity; ++n)
            {
                Inventory->AddItem(Slot.ItemType);
            }
        }
    }
}

void UTPTSaveGameManager::ApplyPlayerEffect(UAbilitySystemComponent* ASC, const int32 Data)
{
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CoreEnergyEffect, 1, ASC->MakeEffectContext());
    if (SpecHandle.IsValid())
    {
        FGameplayTag CoreEnergyTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoreEnergy;
        SpecHandle.Data->SetSetByCallerMagnitude(CoreEnergyTag, Data);

        ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
    }
}
