// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStaminaWidget.h"
#include "Components/ProgressBar.h"

void UPlayerStaminaWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerStaminaWidget::SetStamina(int32 Stamina)
{
	if (!StaminaBar) return;

	float StaminaRatio = FMath::Clamp(static_cast<float>(Stamina) / 100.0f, 0.0f, 1.0f);
	StaminaBar->SetPercent(StaminaRatio);
}
