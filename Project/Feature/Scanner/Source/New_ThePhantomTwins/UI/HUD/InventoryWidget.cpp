// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "ItemSlotWidget.h"
#include "ItemToolTipWidget.h"
#include "Components/WrapBox.h"
#include "../../Objects/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (ItemToolTipWidget)
    {
        ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventoryWidget::LoadInventory(class UInventoryComponent* Inventory)
{
	if (!WrapBox || !Inventory || !ItemSlotWidgetClass) return;

    APlayerController* PC = GetOwningPlayer();

    WrapBox->ClearChildren();

    for (int32 Index = 0; Index < Inventory->MaxInventorySlots; ++Index)
    {
        UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(PC, ItemSlotWidgetClass);

        if (SlotWidget)
        {
            SlotWidget->ResetItemSlot();
            WrapBox->AddChildToWrapBox(SlotWidget);
        }
    }
}

void UInventoryWidget::LoadInventory(const int32 InventorySlotsNum)
{
    if (!WrapBox || !ItemSlotWidgetClass) return;

    APlayerController* PC = GetOwningPlayer();

    WrapBox->ClearChildren();

    for (int32 Index = 0; Index < InventorySlotsNum; ++Index)
    {
        UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(PC, ItemSlotWidgetClass);

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

void UInventoryWidget::SetToolTips(bool bVisible, EItemType eItemType)
{
    if (!ItemToolTipWidget) return;
    
    if (bVisible)
    {
        ItemToolTipWidget->SetVisibility(ESlateVisibility::Visible);

        ItemToolTipWidget->SetItemName(eItemType);
        ItemToolTipWidget->SetItemDescription(eItemType);
    }
    else
    {
        ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
    }


}
