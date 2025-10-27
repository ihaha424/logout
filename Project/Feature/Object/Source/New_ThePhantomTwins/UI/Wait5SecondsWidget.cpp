
#include "Wait5SecondsWidget.h"
#include "Components/ProgressBar.h"

void UWait5SecondsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    DisplayTime = MaxTime;
    RemainingTime = MaxTime;
}

void UWait5SecondsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 선형 보간으로 DisplayStamina 업데이트
    DisplayTime = FMath::FInterpTo(DisplayTime, RemainingTime, InDeltaTime, TimeInterpSpeed);

    if (TimeBar)
    {
        float TimeRatio = FMath::Clamp(RemainingTime / MaxTime, 0.0f, 1.0f);
        TimeBar->SetPercent(TimeRatio);
    }
}

void UWait5SecondsWidget::SetTime(int32 time)
{
    // 목표값만 세팅
    RemainingTime = FMath::Clamp(static_cast<float>(time), 0.0f, MaxTime);
}
