// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStaminaWidget.h"
#include "Components/ProgressBar.h"

void UPlayerStaminaWidget::NativeConstruct()
{
    Super::NativeConstruct();
    DisplayStamina = MaxStamina;
    TargetStamina = MaxStamina;
}

void UPlayerStaminaWidget::SetStamina(int32 Stamina)
{
    // 목표값만 세팅
    TargetStamina = FMath::Clamp(static_cast<float>(Stamina), 0.0f, MaxStamina);
}

void UPlayerStaminaWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 선형 보간으로 DisplayStamina 업데이트
    DisplayStamina = FMath::FInterpTo(DisplayStamina, TargetStamina, InDeltaTime, StaminaInterpSpeed);

    if (StaminaBar)
    {
        float StaminaRatio = FMath::Clamp(DisplayStamina / MaxStamina, 0.0f, 1.0f);
        StaminaBar->SetPercent(StaminaRatio);
    }
}