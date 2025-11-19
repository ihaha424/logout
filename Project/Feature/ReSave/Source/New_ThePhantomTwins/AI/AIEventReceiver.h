// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIEventReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UAIEventReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NEW_THEPHANTOMTWINS_API IAIEventReceiver
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIEvent")
	void ApplyStun();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIEvent")
	void ApplyDie();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIEvent")
	void ApplyDestroy();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AIEvent")
	void ApplyRespawn();
};
