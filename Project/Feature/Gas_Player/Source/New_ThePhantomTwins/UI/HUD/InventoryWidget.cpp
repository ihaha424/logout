// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "ItemSlotWidget.h"
#include "Components/WrapBox.h"
#include "../../Objects/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::LoadInventory(class UInventoryComponent* Inventory)
{
	// if (!WrapBox || !Inventory || !ItemSlotWidgetClass) return;

    // 테스트용 임시 코드
    if (Inventory == nullptr)   
    {
        if (!WrapBox || !ItemSlotWidgetClass) return;
        
        WrapBox->ClearChildren();

        for (int32 Index = 0; Index < 5; ++Index)
        {
            UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetWorld(), ItemSlotWidgetClass);

            if (SlotWidget)
            {
                SlotWidget->ResetItemSlot();
                WrapBox->AddChildToWrapBox(SlotWidget);
            }
        }

        return;
    }


    WrapBox->ClearChildren();

    for (int32 Index = 0; Index < Inventory->MaxInventorySlots; ++Index)
    {
        UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetWorld(), ItemSlotWidgetClass);

        if (SlotWidget)
        {
            SlotWidget->ResetItemSlot();
            WrapBox->AddChildToWrapBox(SlotWidget);
        }
    }
}

class UItemSlotWidget* UInventoryWidget::GetItemSlotWidget(int32 Index)
{
    if (!WrapBox || Index < 0 || Index >= WrapBox->GetChildrenCount())
        return nullptr;

    return Cast<UItemSlotWidget>(WrapBox->GetChildAt(Index));
}
