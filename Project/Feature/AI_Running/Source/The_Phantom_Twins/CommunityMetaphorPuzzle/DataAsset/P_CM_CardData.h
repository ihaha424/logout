// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "P_CM_CardData.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_CardData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	FText	Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	int32	LogicValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	int32	EmotionValue;
};
 