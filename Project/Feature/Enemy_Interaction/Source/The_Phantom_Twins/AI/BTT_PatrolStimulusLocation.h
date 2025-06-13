// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PatrolStimulusLocation.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTT_PatrolStimulusLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_PatrolStimulusLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 5.f;

private:
};
