// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PatrolSplineRoute.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Move | PatrolSplineRoute"))
class THE_PHANTOM_TWINS_API UBTT_PatrolSplineRoute : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_PatrolSplineRoute();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

private:
	UPROPERTY(EditAnywhere, Category = "AI|Spline")
	float MoveSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI|Spline")
	bool bIsFollowingSpline = false;

	int MaxIndex = 0;
	int Index = 0;
};
