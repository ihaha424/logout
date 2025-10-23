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
            USaveIDComponent* SaveIDComp = Door->FindComponentByClass<USaveIDComponent>();
            if (!IsValid(SaveIDComp))
            {
                continue;
            }
            FGuid DoorID = SaveIDComp->SaveId;

            if (DoorActorsMap.Contains(DoorID))
            {
                DoorActorsMap[DoorID] = Door;
            }
 
            if (!bActorsInitialized)
            {
                DoorActorsMap.Add(DoorID, Door);
            	FDoorState DoorState;
				DoorState.bIsOpened = Door->bIsActived;
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
            USaveIDComponent* SaveIDComp = Item->FindComponentByClass<USaveIDComponent>();
            if (!IsValid(SaveIDComp))
            {
                continue;
            }
            FGuid ItemID = SaveIDComp->SaveId;

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
            USaveIDComponent* SaveIDComp = HideObject->FindComponentByClass<USaveIDComponent>();
            if (!IsValid(SaveIDComp))
            {
                continue;
            }
            FGuid HideObjectID = SaveIDComp->SaveId;

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
            USaveIDComponent* SaveIDComp = Box->FindComponentByClass<USaveIDComponent>();
            if (!IsValid(SaveIDComp))
            {
                continue;
            }
            FGuid BoxID = SaveIDComp->SaveId;

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
            USaveIDComponent* SaveIDComp = AI->FindComponentByClass<USaveIDComponent>();
            FGuid AIID = SaveIDComp->SaveId;

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

void UTPTSaveGameManager::TempSaveByID(const FGuid& ObjectID, const bool bIsExist)
{
    if (!GameSaveGame)
        return;

    // 문 액터에서 매칭되는 ObjectID가 있으면 상태 업데이트
    if (AActor** DoorActor = DoorActorsMap.Find(ObjectID))
    {
        ADoor* Door = Cast<ADoor>(*DoorActor);
        if (Door)
        {
            FDoorState DoorState;
            DoorState.bIsOpened = Door->bIsActived;
            DoorState.bIsExist = bIsExist;
            GameSaveGame->DoorStates[ObjectID] = DoorState;
            return;
        }
    }

    // 아이템 액터 상태 업데이트
    if (AActor** ItemActor = ItemActorsMap.Find(ObjectID))
    {
        AItemObject* Item = Cast<AItemObject>(*ItemActor);
        if (Item)
        {
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

void UTPTSaveGameManager::SaveRestartPoint(const FVector& PlayerLocation, const FRotator& PlayerRotation)
{
	GameSaveGame->PlayerLocation = PlayerLocation;
	GameSaveGame->PlayerRotation = PlayerRotation;
}

void UTPTSaveGameManager::ApplyActorSaveGame()
{
    if (!GameSaveGame)
        return;

    // 문 상태 적용
    for (const auto& DoorPair : GameSaveGame->DoorStates)
    {
        FGuid DoorID = DoorPair.Key;
        FDoorState DoorState = DoorPair.Value;

        if (AActor** ActorPtr = DoorActorsMap.Find(DoorID))
        {
            ADoor* Door = Cast<ADoor>(*ActorPtr);
            if (!DoorPair.Value.bIsExist)
            {
				Door->Destroy();
            }
        	Door->SetActive(DoorState.bIsOpened);
        }
    }

    // 아이템 상태 적용
    for (const auto& ItemPair : GameSaveGame->ItemStates)
    {
        FGuid ItemID = ItemPair.Key;
        bool bIsExist = ItemPair.Value;

        if (AActor** ActorPtr = ItemActorsMap.Find(ItemID))
        {
            AItemObject* Item = Cast<AItemObject>(*ActorPtr);
            if (!bIsExist)
            {
                Item->DestroyItem();
            }
        }
    }

    // 숨는 오브젝트 상태 적용
    for (const auto& HidePair : GameSaveGame->HideObjectStates)
    {
        FGuid HideID = HidePair.Key;
        bool bIsExist = HidePair.Value;

        if (AActor** ActorPtr = HideObjectActorsMap.Find(HideID))
        {
            AInteractHideObject* HideObj = Cast<AInteractHideObject>(*ActorPtr);
            if (!bIsExist)
            {
                HideObj->Destroy();
            }
        }
    }

    // 아이템 박스 상태 적용
    for (const auto& BoxPair : GameSaveGame->ItemBoxStates)
    {
        FGuid BoxID = BoxPair.Key;
        bool bIsOpened = BoxPair.Value;

        if (AActor** ActorPtr = ItemBoxActorsMap.Find(BoxID))
        {
            ABoxObject* Box = Cast<ABoxObject>(*ActorPtr);
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
        FGuid AIID = AIPair.Key;
        bool bIsExist = AIPair.Value;

        if (AActor** ActorPtr = AIActorsMap.Find(AIID))
        {
            AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(*ActorPtr);
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
