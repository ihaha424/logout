// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleFileBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API USimpleFileBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "SimpleFile")
	static bool LoadCodeText(FString& OutText);
};
