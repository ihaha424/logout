// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UBTD_CheckDistanceToTarget : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTD_CheckDistanceToTarget();

protected:
	virtual FString GetStaticDescription() const override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	//~ Begin Blackboard Key
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ObjectActorKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bSmashObjectKey;
	//~ End Blackboard Key


	//~ Begin Proterty
	// 시야 감지 임계값
	UPROPERTY(EditAnywhere)
	float Distance = 200.f;

	UPROPERTY(EditAnywhere)
	float DistanceThreshold = 50.f;
	//~ End Proterty

};
