#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Wait5SecondsWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UWait5SecondsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    void SetTime(int32 time);

public:
    UPROPERTY()
    float MaxTime = 5.0f;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> TimeBar;

    // 추가: Widget에 표시할 Time
    float DisplayTime = 5.0f; // 시작값을 최대치로
    float RemainingTime = 5.0f;
    float TimeInterpSpeed = 0.5f; // 보간 속도 (원하는 부드러움 정도로 조절)
};
