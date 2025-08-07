// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CharacterType.h"
#include "TPTSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FIdentifyCharacterData IdentifyCharacterData;
};
