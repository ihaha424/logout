
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PS_Player.h"

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

	bCanInteract = bIsDetected;

	SetWidgetVisible(Interactor, bCanInteract);

	return bCanInteract;
}

void AInventoryItem::SetWidgetVisible(const APawn* Interactor, bool bVisible)
{
    // 안전하게 PlayerState 캐스트 (nullptr 검사)
    const APS_Player* PS = nullptr;
    if (Interactor)
    {
        PS = Interactor->GetPlayerState<APS_Player>();
    }

    // 각 컴포넌트가 없을 수 있으니, 존재할 때만 동작하게 변경
    if (!bVisible)
    {
        if (InteractWidgetComp)
        {
            InteractWidgetComp->SetVisibility(false);
        }
        if (FullWidgetComp)
        {
            FullWidgetComp->SetVisibility(false);
        }

        // bVisible == false 면 여기서 끝
        return;
    }

    // bVisible == true 인 경우 (보이게 할 때)
    // 플레이어 혹은 PlayerState가 없으면 기본적으로 Interact 보여주기 (혹은 둘다 숨기기) 의 정책을 정하세요.
    // 아래는 PlayerState/InventoryComp가 있고, 인벤토리가 꽉 찼는지 확인하는 안전한 체크 예시입니다.
    bool bInventoryFull = false;
    if (PS && PS->InventoryComp)
    {
        bInventoryFull = PS->InventoryComp->IsInventoryFull();
    }

    if (bInventoryFull)
    {
        if (InteractWidgetComp) InteractWidgetComp->SetVisibility(false);
        if (FullWidgetComp) FullWidgetComp->SetVisibility(true);

        bCanInteract = false;
        OnInventoryFull();
    }
    else
    {
        if (InteractWidgetComp) InteractWidgetComp->SetVisibility(true);
        if (FullWidgetComp) FullWidgetComp->SetVisibility(false);

        // 상호작용 가능 상태 유지
        bCanInteract = true;
    }
}

void AInventoryItem::OnInteractServer_Implementation(const APawn* Interactor)
{
	InvokeGameplayCue(Interactor);	// 자기 자신 이펙트 재생
	
	// 상호작용한 플레이어의 Inventory에 EItemType 넘김.
    if (Interactor)
    {
        if (APS_Player* PS = Interactor->GetPlayerState<APS_Player>())
        {
            if (PS->InventoryComp)
            {
                PS->InventoryComp->AddItem(ItemType);
            }
        }
    }

	DestroyItem();
}
