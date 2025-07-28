
#include "InventoryComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../UI/HUD/PlayerHUDWidget.h"
#include "../UI/HUD/InventoryWidget.h"
#include "../UI/HUD/ItemSlotWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

    InventorySlots.Init(FItemSlot(), MaxInventorySlots);
}

void UInventoryComponent::AddItem(EItemType eItemType)
{
    int32 EmptySlotIndex = INDEX_NONE;

    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        // 같은 아이템을 찾음.
        if (InventorySlots[i].ItemType == eItemType)
        {
            // 같은 아이템이 있고 스택이 최대 미만이면
            if (InventorySlots[i].ItemQuantity < MaxQuantity)
            {
                // 스택 증가
                InventorySlots[i].ItemQuantity++;

                // 인벤토리 위젯 : 수량 텍스트 변경
                if (PlayerHUDWidget.IsValid())
                {
                    PlayerHUDWidget->SetItemQuantity(i, InventorySlots[i].ItemQuantity);
                }

                return;
            }
            // 스택이 가득 찬 동일 아이템이어도, 처리하지 않고 계속 탐색
        }
        else if (InventorySlots[i].ItemQuantity == 0 && EmptySlotIndex == INDEX_NONE)
        {
            // 처음 만나는 빈 칸의 인덱스 저장 (추가는 나중에)
            EmptySlotIndex = i;
        }
    }

    // 루프 끝까지 같은 아이템 못 찾음 == 지금까지 아무 것도 추가 안됨
    // 빈 칸이 있으면 빈 칸에 신규로 추가
    if (EmptySlotIndex != INDEX_NONE)
    {
        InventorySlots[EmptySlotIndex].ItemType = eItemType;
        InventorySlots[EmptySlotIndex].ItemQuantity = 1;

        // 인벤토리 위젯 변경
        if (PlayerHUDWidget.IsValid())
        {
            // 아이템 아이콘 변경
            PlayerHUDWidget->SetItemIcon(EmptySlotIndex, eItemType);

            // 수량 텍스트 변경
            PlayerHUDWidget->SetItemQuantity(EmptySlotIndex, 1);
        }


        return;
    }
    
    // 여기까지 왔으면 빈 칸도 없고, 기존 아이템도 스택 꽉 찬 상태 => 더 이상 넣을 수 없음
}

EItemType UInventoryComponent::UseItem(int32 SlotIndex)
{
    // 예외처리 (슬롯 인덱스 제외한 숫자가 들어 온 경우.)
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return EItemType::None;
    }

    FItemSlot& itemSlot = InventorySlots[SlotIndex - 1];

    // itemSlot.ItemQuantity가 0 이면(빈 슬롯이면) : return EItemType::None;
    if (itemSlot.ItemQuantity <= 0 || itemSlot.ItemType == EItemType::None)
    {
        return EItemType::None;
    }

    // 현재 아이템 타입 반환 (사용한 아이템 종류)
    EItemType usedItemType = itemSlot.ItemType;

    // itemSlot.ItemQuantity가 2 이상이면 아이템 수량 감소 처리
    if (itemSlot.ItemQuantity > 1)
    {
        itemSlot.ItemQuantity--;

        // 인벤토리 위젯 : 수량 텍스트 변경
        if (PlayerHUDWidget.IsValid())
        {
            PlayerHUDWidget->SetItemQuantity(SlotIndex - 1, itemSlot.ItemQuantity);
        }
    }
    else
    {
        // 수량이 1이면 아이템 제거, 슬롯 초기화
        itemSlot.ItemType = EItemType::None;
        itemSlot.ItemQuantity = 0;

        // 인벤토리 위젯 : 초기화
        PlayerHUDWidget->ResetItemSlot(SlotIndex - 1);
    }

    return usedItemType;
}

bool UInventoryComponent::SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget)
{
    // 인자로 받은 위젯을 먼저 저장
    PlayerHUDWidget = HUDWidget;

    // OwnerPlayer에서 현재 위젯을 다시 받아옴
    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
    if (!OwnerPlayer)
    {
        return false; // Owner가 없으면 false
    }

    UPlayerHUDWidget* TempWidget = OwnerPlayer->GetPlayerHUDWidget();

    // 두 포인터가 같은 객체를 가리키는지 비교해서 반환
    return (PlayerHUDWidget.Get() == TempWidget);
}

