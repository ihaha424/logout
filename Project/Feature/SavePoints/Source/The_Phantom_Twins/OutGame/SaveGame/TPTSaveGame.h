// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPTSaveGameData.h"
#include "GameFramework/SaveGame.h"
#include "TPTSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UTPTSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UTPTSaveGame();

	UPROPERTY()
	TArray<FPlayerSaveData> Players;

	UPROPERTY()
	ECharacterType HostSelectedCharacter;

	UPROPERTY()
	ECharacterType ClientSelectedCharacter;
};
