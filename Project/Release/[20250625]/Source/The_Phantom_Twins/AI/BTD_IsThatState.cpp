// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsThatState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsThatState::UBTD_IsThatState()
{
	NodeName = TEXT("Is That State");
}

bool UBTD_IsThatState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
		return false;

	uint8 StateValue = BlackboardComp->GetValueAsEnum("AIState");
	return StateValue == static_cast<uint8>(AIState);
}