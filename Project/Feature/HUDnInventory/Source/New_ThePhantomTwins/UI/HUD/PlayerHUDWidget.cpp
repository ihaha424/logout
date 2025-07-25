// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Blueprint/UserWidget.h"

#include "PlayerStatusWidget.h"
#include "ClearItemCounterWidget.h"
#include "PlayerStaminaWidget.h"
#include "PlayerSkillWidget.h"
#include "InventoryWidget.h"

#include "../../Log/TPTLog.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerHUDWidget::InitializeWidgets(int32 HP/*=100*/, int32 Mental/*=100*/, int32 Stamina/*=100*/, int32 CoreEnergyNum/*=5*/, int32 MaxInventorySlots/*=5*/, UTexture2D* PortraitTexture/*=nullptr*/, UTexture2D* ActiveSkillIcon /*= nullptr*/, UTexture2D* PassiveSkillIcon /*= nullptr*/)
{
/* PlayerStatus */
    UpdateHP(HP);
    UpdateMental(Mental);
    SetCharPortrait(PortraitTexture);
    UpdateStamina(Stamina);

/* Skill */
    UpdateCoreEnergy(CoreEnergyNum);
    SetActiveSkillIcon(ActiveSkillIcon);
    SetPassiveSkillIcon(PassiveSkillIcon);

/* Inventory */
    SetMaxInventorySlots(MaxInventorySlots);
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

void UPlayerHUDWidget::SetMaxInventorySlots(const int32 MaxInventorySlots)
{
    if (InventoryWidget)
    {
        InventoryWidget->LoadInventory(MaxInventorySlots);
    }
}
