// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Player/CharacterType.h"
#include "IdentifyChracterData.generated.h"


UCLASS(BlueprintType)
class THE_PHANTOM_TWINS_API UIdentifyChracterData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IdentifyChracter")
	ECharacterType Host = ECharacterType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IdentifyChracter")
	ECharacterType Client = ECharacterType::None;
};
