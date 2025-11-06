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
    LevelSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLevelSaveGame>("MainSlot",  0, true);
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

void UTPTSaveGameManager::ReInitialize()
{
    bActorsInitialized = false;
    bPlayerInitialized = false;

    UTPTSaveGameHelperLibrary::DeleteSaveGameData<UTPTLevelSaveGame>();
	SetRestartPoint(nullptr);

    DoorActorsMap.Reset();
    ItemActorsMap.Reset();
    HideObjectActorsMap.Reset();
    ItemBoxActorsMap.Reset();
    AIActorsMap.Reset();

    SaveUpdate();
}

void UTPTSaveGameManager::FullInitialize()
{
    UTPTSaveGameHelperLibrary::DeleteSaveGameData<UTPTLevelSaveGame>();
    UTPTSaveGameHelperLibrary::DeleteSaveGameData<UTPTLocalPlayerSaveGame>();
    PlayerSaveGames.Reset();

    LevelSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLevelSaveGame>("MainSlot", 0, true);
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>("MainSlot", 0, true));
    PlayerSaveGames.Add(UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLocalPlayerSaveGame>("MainSlot", 0, true));

    DoorActorsMap.Reset();
    ItemActorsMap.Reset();
    HideObjectActorsMap.Reset();
    ItemBoxActorsMap.Reset();
    AIActorsMap.Reset();
}

void UTPTSaveGameManager::Deinitialize()
{
    UTPTSaveGameHelperLibrary::DeleteSaveGameData<UTPTLevelSaveGame>();
    UTPTSaveGameHelperLibrary::DeleteSaveGameData<UTPTLocalPlayerSaveGame>();

    PlayerSaveGames.Reset();
    LevelSaveGame = nullptr;

    DoorActorsMap.Reset();
    ItemActorsMap.Reset();
    HideObjectActorsMap.Reset();
    ItemBoxActorsMap.Reset();
    AIActorsMap.Reset();

    Super::Deinitialize();
}

void UTPTSaveGameManager::SaveUpdate()
{
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLevelSaveGame>(LevelSaveGame);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[0]);
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTLocalPlayerSaveGame>(PlayerSaveGames[1]);
}

void UTPTSaveGameManager::InitializeSaveTargets()
{
    if (!LevelSaveGame)
    {
        return;
    }

    LevelSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLevelSaveGame>("MainSlot", 0, false);

    TArray<AActor*> TempActors;
    // 문 액터
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), TempActors);
    for (AActor* Actor : TempActors)
    {
        if (Actor)
        {
            ADoor* Door = Cast<ADoor>(Actor);
            FName DoorID = Door->GetFName();
            TPT_LOG(SaveGameLog, Warning, TEXT("DataFragment : %s"), *DoorID.ToString())
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
                LevelSaveGame->DoorStates.FindOrAdd(DoorID, DoorState);
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
            FName ItemID = Item->GetFName();

            if (ItemActorsMap.Contains(ItemID))
            {
                ItemActorsMap[ItemID] = Item;
            }

            if (!bActorsInitialized)
            {
                ItemActorsMap.Add(ItemID, Item);
                LevelSaveGame->ItemStates.FindOrAdd(ItemID, true);
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
            FName HideObjectID = HideObject->GetFName();

            if (HideObjectActorsMap.Contains(HideObjectID))
            {
                HideObjectActorsMap[HideObjectID] = HideObject;
            }

            if (!bActorsInitialized)
            {
                HideObjectActorsMap.Add(HideObjectID, HideObject);
                LevelSaveGame->HideObjectStates.FindOrAdd(HideObjectID, true);
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
            FName BoxID = Box->GetFName();

            if (ItemBoxActorsMap.Contains(BoxID))
            {
                ItemBoxActorsMap[BoxID] = Box;
            }

            if (!bActorsInitialized)
            {
                ItemBoxActorsMap.Add(BoxID, Box);
                LevelSaveGame->ItemBoxStates.FindOrAdd(BoxID, Box->bIsActived);
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
            FName AIID = AI->GetFName();

            if (AIActorsMap.Contains(AIID))
            {
                AIActorsMap[AIID] = AI;
            }

            if (!bActorsInitialized)
            {
                AIActorsMap.Add(AIID, AI);
                LevelSaveGame->AIStates.FindOrAdd(AIID, true);
            }
        }
    }
    bActorsInitialized = true;
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

    SetRestartPoint(nullptr);
    bPlayerInitialized = true;
}

void UTPTSaveGameManager::TempSaveByID(const FName ObjectID, const bool bIsExist)
{
    if (!LevelSaveGame)
    {
        return;
    }
    // 문 액터에서 매칭되는 ObjectID가 있으면 상태 업데이트
    if (DoorActorsMap.Contains(ObjectID))
    {
        AActor** DoorActor = DoorActorsMap.Find(ObjectID);
        ADoor* Door = Cast<ADoor>(*DoorActor);

        if (Door)
        {
            FDoorState DoorState;
            DoorState.bIsUnLocked = Door->bIsAllTriggered;
            DoorState.bIsOpen = Door->bIsActived;
            DoorState.bIsExist = bIsExist;
            LevelSaveGame->DoorStates[ObjectID] = DoorState;
            return;
        }
    }

    // 아이템 액터 상태 업데이트
    if (ItemActorsMap.Contains(ObjectID))
    {
        AActor** ItemActor = ItemActorsMap.Find(ObjectID);
        AItemObject* Item = Cast<AItemObject>(*ItemActor);

        if (Item)
        {
            LevelSaveGame->ItemStates[ObjectID] = bIsExist;
            return;
        }
    }

    // 숨는 오브젝트 상태 업데이트
    if (HideObjectActorsMap.Contains(ObjectID))
    {
        AActor** HideActor = HideObjectActorsMap.Find(ObjectID);
        AInteractHideObject* HideObj = Cast<AInteractHideObject>(*HideActor);
        if (HideObj)
        {
            LevelSaveGame->HideObjectStates[ObjectID] = bIsExist;
            return;
        }
    }

    // 아이템 박스 상태 업데이트
    if (ItemBoxActorsMap.Contains(ObjectID))
    {
        AActor** ItemBoxActor = ItemBoxActorsMap.Find(ObjectID);
        ABoxObject* Box = Cast<ABoxObject>(*ItemBoxActor);
        if (Box)
        {
            bool bIsOpened = Box->bIsActived;
            LevelSaveGame->ItemBoxStates[ObjectID] = bIsOpened;

            return;
        }
    }

    // AI 상태 업데이트
    if (AIActorsMap.Contains(ObjectID))
    {
        AActor** AIActor = AIActorsMap.Find(ObjectID);
        AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*AIActor);
        if (AI)
        {
            LevelSaveGame->AIStates[ObjectID] = bIsExist;
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
        LevelSaveGame->HostPlayerStart = FTransform::Identity;
        LevelSaveGame->ClientPlayerStart = FTransform::Identity;
        return;
    }

    LevelSaveGame->HostPlayerStart = DataFragment->HostPlayerLocation->GetComponentTransform();
    LevelSaveGame->ClientPlayerStart = DataFragment->ClientPlayerLocation->GetComponentTransform();
}

FTransform UTPTSaveGameManager::GetRestartPoint(const bool bIsHost)
{
    if (bIsHost)
    {
        return LevelSaveGame->HostPlayerStart;
    }
    return LevelSaveGame->ClientPlayerStart;
}

void UTPTSaveGameManager::ApplyActorSaveGame()
{
    LevelSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTLevelSaveGame>("MainSlot", 0, false);

    // 문 상태 적용
    for (const auto& DoorPair : LevelSaveGame->DoorStates)
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
            Door->AreAllTriggerActived();
            Door->OnRep_bIsActived();
        }
    }

    // 아이템 상태 적용
    for (const auto& ItemPair : LevelSaveGame->ItemStates)
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
    for (const auto& HidePair : LevelSaveGame->HideObjectStates)
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
    for (const auto& BoxPair : LevelSaveGame->ItemBoxStates)
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
    for (const auto& AIPair : LevelSaveGame->AIStates)
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
        ApplyPlayerEffect(ASC, PlayerSaveGames[0]->CoreEnergy);

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
