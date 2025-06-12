// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTD_IsAttackRange : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTD_IsAttackRange();
protected:
	// 데코레이션이 반환할 값이 True인지 False인지 결정하는 함수-> 우리가 오버라이드해서 판단조건을 설정해주면 된다.
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float AttackRange;
};
