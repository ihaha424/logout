// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsAttackRange.h"
#include "AIController.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTD_IsAttackRange::UBTD_IsAttackRange()
{
	NodeName = TEXT("IsAttackRange");
}

bool UBTD_IsAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("ChasingPlayer")));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	float DistanceToTarget = AIPawn->GetDistanceTo(Target);
	float AttackRangeWithRadius = 200;
	bool bResult = DistanceToTarget <= AttackRangeWithRadius;
	return bResult;
}
