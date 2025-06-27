// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P_CM_PuzzleState.generated.h"


USTRUCT(BlueprintType)
struct THE_PHANTOM_TWINS_API FPuzzleState
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<bool> bCardFlipped;

    UPROPERTY()
    int32 LogicalValue;

    UPROPERTY()
    int32 EmotionValue;
};

