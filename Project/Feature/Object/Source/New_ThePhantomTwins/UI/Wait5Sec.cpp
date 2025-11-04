// Fill out your copyright notice in the Description page of Project Settings.


#include "Wait5Sec.h"
#include "Components/ProgressBar.h"
#include "Log/TPTLog.h"

void UWait5Sec::NativeConstruct()
{
    Super::NativeConstruct();

    DisplayTime = MaxTime;
    RemainingTime = MaxTime;
}

void UWait5Sec::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);


    // 선형 보간으로 DisplayStamina 업데이트
    DisplayTime = FMath::FInterpTo(DisplayTime, RemainingTime, InDeltaTime, TimeInterpSpeed);

    if (TimeBar)
    {
        RemainingTime -= InDeltaTime;
        float TimeRatio = FMath::Clamp(RemainingTime / MaxTime, 0.0f, 1.0f);
        TimeBar->SetPercent(TimeRatio);
    }
}

void UWait5Sec::SetTime(int32 time)
{
    // 목표값만 세팅
    RemainingTime = FMath::Clamp(static_cast<float>(time), 0.0f, MaxTime);
}

