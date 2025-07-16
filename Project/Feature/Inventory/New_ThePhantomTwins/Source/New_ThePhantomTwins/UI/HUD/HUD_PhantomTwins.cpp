
#include "HUD_PhantomTwins.h"
#include "Blueprint/UserWidget.h"

#include "PlayerStatusWidget.h"
#include "ClearItemCounterWidget.h"
#include "PlayerStaminaWidget.h"
#include "PlayerSkillWidget.h"
#include "InventoryWidget.h"
#include "../../Log/TPTLog.h"


void AHUD_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();

    if (AimDotWidgetClass)
    {
        AimDotWidget = CreateWidget<UUserWidget>(GetWorld(), AimDotWidgetClass);
        if (AimDotWidget)
        {
            AimDotWidget->AddToViewport();
        }
    }

    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->AddToViewport();
        }
    }

    if (PlayerStaminaWidgetClass)
    {
        PlayerStaminaWidget = CreateWidget<UPlayerStaminaWidget>(GetWorld(), PlayerStaminaWidgetClass);
        if (PlayerStaminaWidget)
        {
            PlayerStaminaWidget->AddToViewport();
        }
    }

    if (ClearItemCounterWidgetClass)
    {
        ClearItemCounterWidget = CreateWidget<UClearItemCounterWidget>(GetWorld(), ClearItemCounterWidgetClass);
        if (ClearItemCounterWidget)
        {
            ClearItemCounterWidget->AddToViewport();
        }
    }

    if (PlayerSkillWidgetClass)
    {
        PlayerSkillWidget = CreateWidget<UPlayerSkillWidget>(GetWorld(), PlayerSkillWidgetClass);
        if (PlayerSkillWidget)
        {
            PlayerSkillWidget->AddToViewport();
        }
    }

    if (InventoryWidgetClass)
    {
        InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
        if (InventoryWidget)
        {
            InventoryWidget->AddToViewport();
        }
    }
}

/* PlayerStatus */
void AHUD_PhantomTwins::UpdateHP(int32 HP)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetHP(HP);
    }
}

void AHUD_PhantomTwins::UpdateMental(int32 Mental)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetMental(Mental);
    }
}

void AHUD_PhantomTwins::SetCharPortrait(UTexture2D* PortraitTexture)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetCharPortrait(PortraitTexture);
    }
}

void AHUD_PhantomTwins::UpdateStamina(int32 Stamina)
{
    if (PlayerStaminaWidget)
    {
        PlayerStaminaWidget->SetStamina(Stamina);
    }
}

/* ClearItem */
void AHUD_PhantomTwins::UpdateClearItem(int32 CurrentClearItem)
{
    if (ClearItemCounterWidget)
    {
        ClearItemCounterWidget->SetClearItemCount(CurrentClearItem);
    }
}
