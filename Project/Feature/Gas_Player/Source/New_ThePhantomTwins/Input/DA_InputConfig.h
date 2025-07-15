// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DA_InputConfig.generated.h"

class UInputAction;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FTaggedInputAction
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
    FGameplayTag InputTag;
};


UCLASS()
class NEW_THEPHANTOMTWINS_API UDA_InputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
    const UInputAction* FindInputActionForTag(const FGameplayTag& InputTag) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FTaggedInputAction> TaggedInputActions;
};
