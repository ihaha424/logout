// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStaminaWidget.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UPlayerStaminaWidget : public UUserWidget
{
    GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    void SetStamina(int32 Stamina);

public:
    UPROPERTY()
    float MaxStamina = 100.0f;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> StaminaBar;

    // 추가: Widget에 표시할 Stamina
    float DisplayStamina = 100.0f; // 시작값을 최대치로
    float TargetStamina = 100.0f;
    float StaminaInterpSpeed = 5.0f; // 보간 속도 (원하는 부드러움 정도로 조절)
};