// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Destroyable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDestroyable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPTOBJECTS_API IDestroyable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destroy")
    bool CanBeDestroyed(const APawn* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destroy")
	void OnDestroy(const APawn* Interactor);
};
