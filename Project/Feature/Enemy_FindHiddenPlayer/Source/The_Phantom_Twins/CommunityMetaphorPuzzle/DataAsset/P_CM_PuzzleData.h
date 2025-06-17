// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "P_CM_CardData.h"
#include "P_CM_CommentData.h"
#include "P_CM_PuzzleData.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_PuzzleData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	int32 LogicValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	int32 EmotionValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TArray<UP_CM_CardData*> CardList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TArray<UP_CM_CommentData*> CommentList;
};
