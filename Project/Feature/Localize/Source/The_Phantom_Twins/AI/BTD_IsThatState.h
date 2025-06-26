// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "AIInterface.h"
#include "BTD_IsThatState.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTD_IsThatState : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTD_IsThatState();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	// BT 에디터에서 선택할 수 있게 Enum Property 추가 (옵션)
	UPROPERTY(EditAnywhere, Category = "Condition")
	EMyAIState AIState;
};
