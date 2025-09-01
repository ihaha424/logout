// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_PriorityStimulus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Log/TPTLog.h"


struct FPriorityStimulusStruct
{
	int32 CurPriority;
	UObject* CurTargetActor;
};

UBTS_PriorityStimulus::UBTS_PriorityStimulus()
{
	NodeName = TEXT("PriorityStimulus");

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTS_PriorityStimulus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

	FPriorityStimulusStruct* PriorityStimulusStruct = (FPriorityStimulusStruct*)NodeMemory;
	PriorityStimulusStruct->CurTargetActor = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	PriorityStimulusStruct->CurPriority = BB->GetValueAsFloat(PriorityKey.SelectedKeyName);
}

void UBTS_PriorityStimulus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

	FPriorityStimulusStruct* PriorityStimulusStruct = (FPriorityStimulusStruct*)NodeMemory;
	int32 Priority = BB->GetValueAsFloat(PriorityKey.SelectedKeyName);
	UObject* TargetActor = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	if (TargetActor == PriorityStimulusStruct->CurTargetActor)
		return;

	if (Priority < PriorityStimulusStruct->CurPriority)
	{
		PriorityStimulusStruct->CurPriority = Priority;
		PriorityStimulusStruct->CurTargetActor = TargetActor;
		BB->SetValueAsBool(RevaluationKey.SelectedKeyName, true);
		return;
	}
	else if (Priority == PriorityStimulusStruct->CurPriority)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

		APawn* AIPawn = AIController->GetPawn();
		NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, );

		FVector MyLoc = AIPawn->GetActorLocation();
		FVector TargetLoc = AIController->GetPathFollowingComponent()->GetCurrentTargetLocation();
		if (TargetLoc == FVector::Zero())
		{
			return;
		}
		const float CurDistance = FVector::Dist(MyLoc, TargetLoc);
		TargetLoc = BB->GetValueAsVector(StimulusLocationKey.SelectedKeyName);
		const float NewDistance = FVector::Dist(MyLoc, TargetLoc);
		if (NewDistance < CurDistance)
		{
			PriorityStimulusStruct->CurPriority = Priority;
			PriorityStimulusStruct->CurTargetActor = TargetActor;
			BB->SetValueAsBool(RevaluationKey.SelectedKeyName, true);
			return;
		}
	}
}

FString UBTS_PriorityStimulus::GetStaticDescription() const
{
	return FString("Revaluation based on priority when 'Stimulus' comes in");
}

uint16 UBTS_PriorityStimulus::GetInstanceMemorySize() const
{
	return sizeof(FPriorityStimulusStruct);
}
