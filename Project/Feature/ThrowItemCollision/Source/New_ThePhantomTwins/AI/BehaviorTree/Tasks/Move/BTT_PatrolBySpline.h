// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BTT_GASBaseTask.h"
#include "BTT_PatrolBySpline.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "TPTTask | Move | PatrolBySpline"))
class NEW_THEPHANTOMTWINS_API UBTT_PatrolBySpline : public UBTT_GASBaseTask
{
	GENERATED_BODY()
public:
	UBTT_PatrolBySpline();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void Execute_TickTaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SplineActorKey;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float AcceptableRadius;
};
