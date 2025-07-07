// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectState.generated.h"


USTRUCT(BlueprintType)
struct FItemStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPickedUp = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestory = false;
};


USTRUCT(BlueprintType)
struct FHideStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hide")
	bool bHasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hide")
	int32 PlayerNum = 0;
};