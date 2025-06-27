// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IPEncryption.generated.h"


#pragma pack(push, 1)
union FCompactIP
{
	struct
	{
		uint8 A, B, C, D;
		uint32 Port;
	} IP;

	uint32 SplitValue[2];
	uint64 CombinedValue;
};
#pragma pack(pop)

UCLASS()
class THE_PHANTOM_TWINS_API UIPEncryption : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FString GetLocalIPAddress(bool bHasPort = false);
	UFUNCTION(BlueprintCallable)
	static FString IP2Code(const FString& IP, bool bHasPort = false);
	UFUNCTION(BlueprintCallable)
	static FString Code2IP(const FString& Code, bool bHasPort = false);

private:
	static FCompactIP IPEncryption(const FString& IP, bool bHasPort = false);
	static FString IPDecryption(const FCompactIP& IP, bool bHasPort = false);
	static FString TransformHexCustom(const FString& Code, bool Direction);
};
