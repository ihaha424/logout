// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "CardKey.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API ACardKey : public ABaseObject
{
	GENERATED_BODY()
	
public:
	ACardKey();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	int32 grade = 0;
};
