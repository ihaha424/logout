
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PS_Player.h"
#include "../Player/PlayerCharacter.h"
#include "SaveGame/SaveIDComponent.h"
#include "SaveGame/TPTSaveGameManager.h"

AInventoryItem::AInventoryItem()
{
	FullWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("FullWidget"));
	FullWidgetComp->SetupAttachment(RootComponent);
	FullWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	FullWidgetComp->SetDrawSize(FVector2D(10, 10));
	FullWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	FullWidgetComp->SetVisibility(false);
}

void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();

	if (FullWidgetComp && FullWidgetClass)
	{
		FullWidgetComp->SetWidgetClass(FullWidgetClass);
		FullWidgetComp->SetVisibility(false);
	}
}

bool AInventoryItem::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
    if (!Interactor->IsLocallyControlled()) return bIsDetected;
    // 거리가 멀어져 감지되지 않은 경우
    if (!bIsDetected)
    {
        bCanInteract = false;
    }
    else
    {
        bCanInteract = true;
    }

    SetWidgetVisible(Interactor, bCanInteract, ItemType);


    return bCanInteract;
}

void AInventoryItem::SetWidgetVisible(const APawn* Interactor, bool bVisible, EItemType eItemType)
{
    if (!Interactor->IsLocallyControlled()) return;

    // 안전하게 PlayerState 캐스트 (nullptr 검사)
    const APS_Player* PS = nullptr;
    if (Interactor)
    {
        PS = Interactor->GetPlayerState<APS_Player>();
    }

    if (PS && PS->InventoryComp)
    {
        bInventoryFull = !(PS->InventoryComp->CanAddToInventory(eItemType));
    }


    if (!InteractWidgetComp || !FullWidgetComp) return;


    if (!bVisible)
    {
        InteractWidgetComp->SetVisibility(false);
        FullWidgetComp->SetVisibility(false);
        return;
    }


    if (bInventoryFull)
    {
        InteractWidgetComp->SetVisibility(false);
        FullWidgetComp->SetVisibility(true);
    }
    else
    {
        InteractWidgetComp->SetVisibility(true);
        FullWidgetComp->SetVisibility(false);
    }
}

void AInventoryItem::OnInteractServer_Implementation(const APawn* Interactor)
{
    // 안전하게 PlayerState 캐스트 (nullptr 검사)
    const APS_Player* PS = nullptr;
    if (Interactor)
    {
        PS = Interactor->GetPlayerState<APS_Player>();
    }

    if (PS && PS->InventoryComp)
    {
        bInventoryFull = !(PS->InventoryComp->CanAddToInventory(ItemType));
    }

    if (bInventoryFull)
    {
        OnInventoryFull(Interactor);
        return;
    }

	InvokeGameplayCue(Interactor);	// 자기 자신 이펙트 재생
	
	// 상호작용한 플레이어의 Inventory에 EItemType 넘김.
    if (Interactor)
    {
		if (PS->InventoryComp)
		{
			PS->InventoryComp->AddItem(ItemType);
		}
    }

    UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
    SaveGameManager->TempSaveByID(FindComponentByClass<USaveIDComponent>()->SaveId, false);

	DestroyItem();
}
