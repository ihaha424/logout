
#include "InventoryComponent.h"

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
        return;
    }

    // 여기까지 왔으면 빈 칸도 없고, 기존 아이템도 스택 꽉 창 상태! => 더 이상 넣을 수 없음
}

/*
	1. 빈 칸이 없는 경우
		- 아이템이 이미 있다
			- 스택이 최대치(이상)면 넘김(다음 슬롯 확인)
			- 스택이 최대치 "미만"이면 해당 슬롯의 수량을 증가 (Quantity++)
		- 아이템이 없다
			- 아무것도 하지 않고 return

	2. 빈 칸이 있는 경우
		- 아이템이 이미 있다
			- 스택이 최대치(이상)면, 비어있는 슬롯에 새로운 아이템 추가 (Quantity=1)
			- 스택이 최대치 "미만"이면 해당 슬롯의 수량을 증가시키기 (Quantity++)
		- 아이템이 없다
			- 비어있는 슬롯에 새로운 아이템 추가 (Quantity=1)
*/