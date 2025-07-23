// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_FindClosestSplinePath.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Utility | FindClosestSplinePath"))
class NEW_THEPHANTOMTWINS_API UBTT_FindClosestSplinePath : public UBTT_GASBaseTask
{
	GENERATED_BODY()
public:
	UBTT_FindClosestSplinePath();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SplineActorKey;
};
