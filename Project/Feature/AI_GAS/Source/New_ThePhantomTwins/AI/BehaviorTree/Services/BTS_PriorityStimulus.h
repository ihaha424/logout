// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_PriorityStimulus.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UBTS_PriorityStimulus : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_PriorityStimulus();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	//~ Begin Blackboard Key
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PriorityKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RevaluationKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StimulusLocationKey;
	//~ End Blackboard Key

	int32 CurPriority = TNumericLimits<int32>::Max();
};
