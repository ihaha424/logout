// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserCountGameMode.h"
#include "GM_Endding.generated.h"

/**
 *
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_Endding : public AUserCountGameMode
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void TravelToLevel(const FName LevelName, bool bAbsolute);
};
