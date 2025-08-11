// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTT_GASBaseTask.generated.h"

UENUM(BlueprintType)
enum class EGASTagCheckMode : uint8
{
    None,           // No check
    Fail,           // If matched, fail
    Succeed,        // If matched, succeed
    Wait,           // If matched, keep Wait
    Abort,          // If matched, abort
    Exception       // If matched, Do Exception Code
};

UENUM(BlueprintType)
enum class EGameplayTagCheckPhase : uint8
{
    None,               // No check
    OnExecuteOnly,      // Check tags only in ExecuteTask
    OnTickOnly,         // Check tags only in TickTask
    ExecuteAndTick      // Check tags in both Execute and Tick
};

class UAbilitySystemComponent;

UCLASS(Abstract)
class NEW_THEPHANTOMTWINS_API UBTT_GASBaseTask : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTT_GASBaseTask();

    /** Write the actual task tick here. */
    virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) { return EBTNodeResult::Succeeded; }
    virtual EBTNodeResult::Type Execute_TaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) { return EBTNodeResult::Succeeded; }
    virtual void Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {}
    virtual void Execute_TickTaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {}

protected:
    UPROPERTY(EditAnywhere, Category = "Tag Blocking")
    EGameplayTagCheckPhase TagCheckPhase;

    UPROPERTY(EditAnywhere, Category = "Tag Blocking")
    EGASTagCheckMode BlockTagMode;

    UPROPERTY(EditAnywhere, Category = "Tag Blocking")
    EGASTagCheckMode RequiredTagMode;


    UPROPERTY(EditAnywhere, Category = "Tag Blocking")
    TArray<FGameplayTag> BlockTags;

    UPROPERTY(EditAnywhere, Category = "Tag Blocking")
    TArray<FGameplayTag> RequiredTags;


private:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) final;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) final;
    EGASTagCheckMode EvaluateTagBlocking(UAbilitySystemComponent* ASC);
};
