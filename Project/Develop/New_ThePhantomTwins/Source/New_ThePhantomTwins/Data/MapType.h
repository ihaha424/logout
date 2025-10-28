// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapType.generated.h"

UENUM(BlueprintType)
enum class EMapType : uint8
{
	ST1		UMETA(DisplayName = "ST1"),
	ST2		UMETA(DisplayName = "ST2"),
	None	UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct NEW_THEPHANTOMTWINS_API FIdentifyMapData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EMapType MapType = EMapType::None;
};