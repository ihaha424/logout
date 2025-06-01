// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsAttackRange::UBTD_IsAttackRange()
{
	NodeName = TEXT("IsAttackRange");
}

bool UBTD_IsAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == AIPawn)
	{
		return false;
	}
	APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Player")));
	if (nullptr == Target)
	{
		return false;
	}
	float DistanceToTarget = AIPawn->GetDistanceTo(Target);
	float AttackRangeWithRadius = 200;
	bool bResult = DistanceToTarget <= AttackRangeWithRadius;
	return bResult;
}
