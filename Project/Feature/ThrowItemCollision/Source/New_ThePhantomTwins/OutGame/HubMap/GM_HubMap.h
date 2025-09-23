// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_HubMap.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_HubMap : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ExitGame(const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void TravelToLevel(const FName LevelName, bool bAbsolute);
};
