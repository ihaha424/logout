// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "P_CM_CommentData.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_CommentData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	FText	Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TObjectPtr<UTexture2D> Icon;
};
