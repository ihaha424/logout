#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Wait5SecondsWidget.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UWait5SecondsWidget : public UUserWidget
{
    GENERATED_BODY()
    
protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    // 기존 함수 유지(필요하면 사용)
    void SetTime(int32 time);

    // 새로 추가: 위젯 스스로 카운트다운을 시작하게 함 (초 단위, float 지원)
    UFUNCTION(BlueprintCallable)
    void StartCountdown(float Duration);

public:
    UPROPERTY()
    float MaxTime = 5.0f;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> TimeBar;

    // 표시용 값들
    float DisplayTime = 5.0f; // 시작값을 최대치로
    float RemainingTime = 5.0f;
    float TimeInterpSpeed = 0.5f; // 보간 속도
    bool bIsCounting = false; // 카운트다운 진행중 여부

    // 카운트다운이 끝났을 때 블루프린트로 후처리할 수 있게 이벤트 제공
    UFUNCTION(BlueprintImplementableEvent, Category = "Wait5Seconds")
    void OnCountdownFinished();
};