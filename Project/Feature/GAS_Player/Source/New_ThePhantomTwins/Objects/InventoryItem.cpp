
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "../Player/PS_Player.h"

AInventoryItem::AInventoryItem()
{
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
