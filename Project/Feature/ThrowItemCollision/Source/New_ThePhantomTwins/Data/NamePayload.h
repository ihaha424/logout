// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NamePayload.generated.h"


UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UNamePayload : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
};
