// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_ChaseTarget.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Move | ChaseTarget"))
class NEW_THEPHANTOMTWINS_API UBTT_ChaseTarget : public UBTT_GASBaseTask
{
	GENERATED_BODY()
	
public:
	UBTT_ChaseTarget();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void Execute_TickTaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type Execute_TaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere, Category = "Chase")
	float AcceptableRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Chase")
	float ChaseTick = 0.2f;

private:
	float CurTick = 0.f;
};
