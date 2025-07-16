// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolBySpline.h"
#include "Navigation/PathFollowingComponent.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

#include "../../../Utility/SplineActor.h"

struct FSplineTaskData
{
	int32 MaxIndex = 0;
	int32 CurIndex = 0;
	FVector Location = FVector::ZeroVector;
};

UBTT_PatrolBySpline::UBTT_PatrolBySpline()
{
	NodeName = TEXT("PatrolBySpline");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PatrolBySpline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!ensureMsgf(BBComp != nullptr, TEXT("[UBTT_PatrolBySpline] BlackboardComponent is missing!")))
		return EBTNodeResult::Failed;

	ASplineActor* SplineActor = Cast<ASplineActor>(BBComp->GetValueAsObject(SplineActorKey.SelectedKeyName));
	if (!ensureMsgf(SplineActor != nullptr, TEXT("[UBTT_PatrolBySpline] SplineActorKey is Cast<ASplineActor> Fail!")))
		return EBTNodeResult::Failed;
	
	USplineComponent* SplineRoute = SplineActor->SplineComponent;
	if (!ensureMsgf(SplineRoute != nullptr, TEXT("[UBTT_PatrolBySpline] SplineActorKey's Spline Component Casting Fail!")))
		return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;


	FSplineTaskData* TaskData = (FSplineTaskData*)NodeMemory;
	TaskData->MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	TaskData->CurIndex = 0;
	TaskData->Location = SplineRoute->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TaskData->Location);
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);

	FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);

	switch (MoveResult)
	{
	case EPathFollowingRequestResult::RequestSuccessful:
		/*
			노드 메모리에 SplineTaskData 저장
		*/
		return EBTNodeResult::InProgress;

	case EPathFollowingRequestResult::Failed:
	case EPathFollowingRequestResult::AlreadyAtGoal:
	default:
		return EBTNodeResult::Failed;
	}

}
void UBTT_PatrolBySpline::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FSplineTaskData* TaskData = (FSplineTaskData*)NodeMemory;
	if (!TaskData)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	EPathFollowingStatus::Type Status = AIController->GetPathFollowingComponent()->GetStatus();
	if (Status == EPathFollowingStatus::Idle || Status == EPathFollowingStatus::Waiting)
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		if (!ensure(BBComp))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		ASplineActor* SplineActor = Cast<ASplineActor>(BBComp->GetValueAsObject(SplineActorKey.SelectedKeyName));
		if (!SplineActor || !SplineActor->SplineComponent)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		TaskData->CurIndex++;

		if (TaskData->CurIndex >= TaskData->MaxIndex)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}

		TaskData->Location = SplineActor->SplineComponent->GetLocationAtSplinePoint(TaskData->CurIndex, ESplineCoordinateSpace::World);

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(TaskData->Location);
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);

		FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
		if (MoveResult.Code != EPathFollowingRequestResult::RequestSuccessful)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

FString UBTT_PatrolBySpline::GetStaticDescription() const
{
	return TEXT("Patrol By SplineActor");
}

uint16 UBTT_PatrolBySpline::GetInstanceMemorySize() const
{
	return sizeof(FSplineTaskData);
}
