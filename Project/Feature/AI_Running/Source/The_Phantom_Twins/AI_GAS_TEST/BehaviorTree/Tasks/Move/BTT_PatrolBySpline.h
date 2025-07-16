// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PatrolBySpline.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "TPTTask | Move | PatrolBySpline"))
class THE_PHANTOM_TWINS_API UBTT_PatrolBySpline : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_PatrolBySpline();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SplineActorKey;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float AcceptableRadius = 50.f;
};
