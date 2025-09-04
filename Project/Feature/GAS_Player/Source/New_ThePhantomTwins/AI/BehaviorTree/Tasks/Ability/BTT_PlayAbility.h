// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BTT_GASBaseTask.h"
#include "GameplayTagContainer.h"
#include "BTT_PlayAbility.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Ability | PlayAbility"))
class NEW_THEPHANTOMTWINS_API UBTT_PlayAbility : public UBTT_GASBaseTask
{
	GENERATED_BODY()

public:
    UBTT_PlayAbility();

    virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual FString GetStaticDescription() const override;

protected:
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTag AbilityTag;

    UPROPERTY(EditAnywhere, Category = "Ability")
    bool bHasTarget;

    /**
     * @brief If need To Target Ability
     */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

private:
    bool bIsWaitingForAbility;
};
