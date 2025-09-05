// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Sona    UMETA(DisplayName = "Sona"),
	Tia     UMETA(DisplayName = "Tia"),
	None	UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct NEW_THEPHANTOMTWINS_API FIdentifyCharacterData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ECharacterType Host = ECharacterType::None;

	UPROPERTY(BlueprintReadOnly)
	ECharacterType Client = ECharacterType::None;
};