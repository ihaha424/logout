// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "AIHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UAIHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AI")
    static void SetAIStateAndTag(
        UBehaviorTreeComponent* OwnerComp,
        UBlackboardComponent* BB,
        EAIBaseState CurState,
        EAIBaseState NextState,
        FName AIStateKeyName);

    UFUNCTION(BlueprintPure, Category = "AI")
    static FGameplayTag GetTagFromAIState(EAIBaseState State);
};
