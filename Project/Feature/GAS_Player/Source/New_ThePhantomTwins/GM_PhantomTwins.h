// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_PhantomTwins.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_PhantomTwins : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void SeverToLevel(const FName LevelName, bool bAbsolute);
};
