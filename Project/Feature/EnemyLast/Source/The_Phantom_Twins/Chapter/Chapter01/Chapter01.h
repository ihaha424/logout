// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Chapter01.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THE_PHANTOM_TWINS_API UChapter01 : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter01")
	int32 SecurityLevel;
};
