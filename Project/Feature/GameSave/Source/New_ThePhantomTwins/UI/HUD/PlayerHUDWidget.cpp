// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "GS_PhantomTwins.h"

#include "PlayerStatusWidget.h"
#include "ClearItemCounterWidget.h"
#include "PlayerStaminaWidget.h"
#include "PlayerSkillWidget.h"
#include "InventoryWidget.h"
#include "ItemSlotWidget.h"

#include "Log/TPTLog.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>())
    {
        ItemChangedHandle = GS->OnCollectedItemCountChanged().AddUObject(
            this, &UPlayerHUDWidget::UpdateClearItem);
    }
}

void UPlayerHUDWidget::NativeDestruct()
{
    if (AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>())
    {
        if (ItemChangedHandle.IsValid())
            GS->OnCollectedItemCountChanged().Remove(ItemChangedHandle);
    }

    Super::NativeDestruct();
}

void UPlayerHUDWidget::InitializeWidgets(int32 HP/*=200*/, int32 Mental/*=100*/, int32 Stamina/*=100*/, int32 CoreEnergyNum/*=5*/, int32 MaxInventorySlots/*=5*/, UTexture2D* PortraitTexture/*=nullptr*/, UTexture2D* ActiveSkillIcon /*= nullptr*/, UTexture2D* PassiveSkillIcon /*= nullptr*/)
{
/* PlayerStatus */
    UpdateHP(HP);
    UpdateMental(Mental);
    SetCharPortrait(PortraitTexture);
    UpdateStamina(Stamina);
    VisibleStamina(false);

/* Skill */
    UpdateCoreEnergy(CoreEnergyNum);
    SetActiveSkillIcon(ActiveSkillIcon);
    SetPassiveSkillIcon(PassiveSkillIcon);

/* Inventory */
    SetMaxInventorySlots(MaxInventorySlots);
    VisibleInventory(false);

// Max Setting
    PlayerStatusWidget->MaxHP = HP;
    PlayerStatusWidget->MaxMental = Mental;
    PlayerStaminaWidget->MaxStamina = Stamina;
}

/* PlayerStatus */
void UPlayerHUDWidget::UpdateHP(const int32 HP)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetHP(HP);
    }
}

void UPlayerHUDWidget::UpdateMental(const int32 Mental)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetMental(Mental);
    }
}

void UPlayerHUDWidget::SetCharPortrait(UTexture2D* PortraitTexture)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetCharPortrait(PortraitTexture);
    }
}

void UPlayerHUDWidget::UpdateStamina(const int32 Stamina)
{
    if (PlayerStaminaWidget)
    {
        PlayerStaminaWidget->SetStamina(Stamina);
    }
}

void UPlayerHUDWidget::VisibleStamina(bool bVisible)
{
    if (PlayerStaminaWidget)
    {
        if (bVisible)
        {
            PlayerStaminaWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            PlayerStaminaWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}


/* ClearItem */
void UPlayerHUDWidget::UpdateClearItem(const int32 CurrentClearItem)
{
    if (ClearItemCounterWidget)
    {
        ClearItemCounterWidget->SetClearItemCount(CurrentClearItem);
    }
}


/* Skill */
void UPlayerHUDWidget::SetActiveSkillIcon(UTexture2D* ActiveSkillIcon)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->SetActiveSkillIcon(ActiveSkillIcon);
    }
}

void UPlayerHUDWidget::SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->SetPassiveSkillIcon(PassiveSkillIcon);
    }
}

void UPlayerHUDWidget::UpdateCoreEnergy(const int32 CoreEnergyNum)
{
    if (PlayerSkillWidget)
    {
        PlayerSkillWidget->ShowCoreEnergy(CoreEnergyNum);
    }
}

void UPlayerHUDWidget::VisibleInventory(bool bVisible)
{
    if (InventoryWidget)
    {
        if (bVisible)
        {
            InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UPlayerHUDWidget::SetMaxInventorySlots(const int32 MaxInventorySlots)
{
    if (InventoryWidget)
    {
        InventoryWidget->LoadInventory(MaxInventorySlots);
    }
}

void UPlayerHUDWidget::SetItemIcon(int32 SlotIndex, EItemType eItemType)
{
    if (InventoryWidget)
    {
        InventoryWidget->GetItemSlotWidget(SlotIndex)->SetItemIcon(eItemType);
    }
}

void UPlayerHUDWidget::SetItemQuantity(int32 SlotIndex, int32 ItemStack)
{
    if (InventoryWidget)
    {
        InventoryWidget->GetItemSlotWidget(SlotIndex)->SetItemQuantity(ItemStack);
    }
}

void UPlayerHUDWidget::ResetItemSlot(int32 SlotIndex)
{
    if (InventoryWidget)
    {
        InventoryWidget->GetItemSlotWidget(SlotIndex)->ResetItemSlot();
    }
}

void UPlayerHUDWidget::SetOutline(int32 SlotIndex, bool bVisible)
{
    if (InventoryWidget)
    {
        InventoryWidget->GetItemSlotWidget(SlotIndex)->SetOutline(bVisible);
    }
}

void UPlayerHUDWidget::SetToolTips(bool bVisible, EItemType eItemType)
{
    if (InventoryWidget)
    {
        InventoryWidget->SetToolTips(bVisible, eItemType);
    }
}
