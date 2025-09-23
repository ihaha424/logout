// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "Abilities/GameplayAbility.h"
#include "BTT_ApplyEffectByTarget.generated.h"

UCLASS(meta = (DisplayName = "TPTTask | Ability | ApplyEffectByTarget"))
class NEW_THEPHANTOMTWINS_API UBTT_ApplyEffectByTarget : public UBTT_GASBaseTask
{
	GENERATED_BODY()
public:
    UBTT_ApplyEffectByTarget();

    virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

protected:
    UPROPERTY(EditAnywhere, Category = "Ability")
    TSubclassOf<class UGameplayEffect> GameplayEffectClass;

    UPROPERTY(EditAnywhere, Category = "Ability")
    bool bHasDuration;

    UPROPERTY(EditAnywhere, Category = "Ability")
    float Duration;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;
};
