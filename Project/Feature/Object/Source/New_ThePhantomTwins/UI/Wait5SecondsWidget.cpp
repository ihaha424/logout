#include "Wait5SecondsWidget.h"
#include "Components/ProgressBar.h"

void UWait5SecondsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    DisplayTime = MaxTime;
    RemainingTime = MaxTime;
    bIsCounting = false;
}

void UWait5SecondsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsCounting)
    {
        // 남은 시간 감소
        RemainingTime = FMath::Max(0.0f, RemainingTime - InDeltaTime);

        // 프로그레스바 업데이트 (직접 비율 적용)
        if (TimeBar && MaxTime > 0.0f)
        {
            float TimeRatio = FMath::Clamp(RemainingTime / MaxTime, 0.0f, 1.0f);
            TimeBar->SetPercent(TimeRatio);
        }

        // DisplayTime 부드럽게 보간 (시각적으로 부드럽게)
        DisplayTime = FMath::FInterpTo(DisplayTime, RemainingTime, InDeltaTime, TimeInterpSpeed);

        // 끝났을 때
        if (RemainingTime <= 0.0f)
        {
            bIsCounting = false;
            // 블루프린트에서 추가 동작 가능
            OnCountdownFinished();
        }
    }
    else
    {
        // 카운트 중이 아니면 기존 보간만 유지(옵션)
        DisplayTime = FMath::FInterpTo(DisplayTime, RemainingTime, InDeltaTime, TimeInterpSpeed);
    }
}

void UWait5SecondsWidget::SetTime(int32 time)
{
    // 목표값만 세팅 (카운트 시작은 하지 않음)
    RemainingTime = FMath::Clamp(static_cast<float>(time), 0.0f, MaxTime);
    DisplayTime = RemainingTime;
    bIsCounting = false;
}

void UWait5SecondsWidget::StartCountdown(float Duration)
{
    MaxTime = FMath::Max(0.0f, Duration);
    RemainingTime = MaxTime;
    DisplayTime = MaxTime;
    bIsCounting = true;
}