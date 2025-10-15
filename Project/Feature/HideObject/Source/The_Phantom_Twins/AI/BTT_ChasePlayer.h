// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTT_ChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_ChasePlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveSpeed = 400.f;
};
