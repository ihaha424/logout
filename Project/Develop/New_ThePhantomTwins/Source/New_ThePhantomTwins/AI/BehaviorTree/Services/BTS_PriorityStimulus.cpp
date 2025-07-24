// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_PriorityStimulus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Log/TPTLog.h"

UBTS_PriorityStimulus::UBTS_PriorityStimulus()
{
	NodeName = TEXT("PriorityStimulus");

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTS_PriorityStimulus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, );


	int32 HearingSum = BB->GetValueAsFloat(PriorityKey.SelectedKeyName);
	if (HearingSum < CurPriority)
	{
		CurPriority = HearingSum;
		BB->SetValueAsBool(RevaluationKey.SelectedKeyName, true);
		return;
	}
	else if (HearingSum == CurPriority)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

		APawn* AIPawn = AIController->GetPawn();
		NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, );

		FVector MyLoc = AIPawn->GetActorLocation();
		FVector TargetLoc = AIController->GetPathFollowingComponent()->GetCurrentTargetLocation();
		if (TargetLoc == FVector::Zero())
		{
			TPT_LOG(AILog, Warning, TEXT("NoPathAi"));
			CurPriority = HearingSum;
			BB->SetValueAsBool(RevaluationKey.SelectedKeyName, true);
			return;
		}
		const float CurDistance = FVector::Dist(MyLoc, TargetLoc);
		TargetLoc = BB->GetValueAsVector(StimulusLocationKey.SelectedKeyName);
		const float NewDistance = FVector::Dist(MyLoc, TargetLoc);
		if (NewDistance < CurDistance)
		{
			CurPriority = HearingSum;
			BB->SetValueAsBool(RevaluationKey.SelectedKeyName, true);
			return;
		}
	}
}

FString UBTS_PriorityStimulus::GetStaticDescription() const
{
	return FString("Revaluation based on priority when 'Stimulus' comes in");
}
