// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHolding : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPTOBJECTS_API IHolding
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetTime();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CalculateGaugePercent(float Elapsed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetHoldingGaugeUI(const APawn* Interactor, bool bVisible);

protected:
	float Time = 0.0f;
};
