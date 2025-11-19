// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TravelWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTravelWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NEW_THEPHANTOMTWINS_API ITravelWidgetInterface
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BeginPlayTravelLevel();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void EndPlayTravelLevel();
};
