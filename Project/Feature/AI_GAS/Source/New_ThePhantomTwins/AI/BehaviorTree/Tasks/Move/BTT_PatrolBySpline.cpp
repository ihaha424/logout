// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolBySpline.h"
#include "Navigation/PathFollowingComponent.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

#include "../../../Utility/SplineActor.h"
#include "Log/TPTLog.h"

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
	NULLCHECK_RETURN_LOG(BBComp, AILog, Warning, EBTNodeResult::Failed)

	ASplineActor* SplineActor = Cast<ASplineActor>(BBComp->GetValueAsObject(SplineActorKey.SelectedKeyName));
	NULLCHECK_RETURN_LOG(SplineActor, AILog, Warning, EBTNodeResult::Failed)
	
	USplineComponent* SplineRoute = SplineActor->SplineComponent;
	NULLCHECK_RETURN_LOG(SplineRoute, AILog, Warning, EBTNodeResult::Failed)

	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AIController, AILog, Warning, EBTNodeResult::Failed)


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
		return EBTNodeResult::InProgress;

	case EPathFollowingRequestResult::AlreadyAtGoal:
		if(TaskData->MaxIndex > 1)
			return EBTNodeResult::InProgress;
		else
			return EBTNodeResult::Failed;
		
	case EPathFollowingRequestResult::Failed:
		TPT_LOG(AILog, Warning, TEXT("EPathFollowingRequestResult: Failed."));
	default:
		return EBTNodeResult::Failed;
	}

}
void UBTT_PatrolBySpline::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FSplineTaskData* TaskData = (FSplineTaskData*)NodeMemory;
	NULLCHECK_CODE_RETURN_LOG(TaskData, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_CODE_RETURN_LOG(AIController, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

	EPathFollowingStatus::Type Status = AIController->GetPathFollowingComponent()->GetStatus();
	if (Status == EPathFollowingStatus::Idle || Status == EPathFollowingStatus::Waiting)
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		NULLCHECK_CODE_RETURN_LOG(BBComp, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

		ASplineActor* SplineActor = Cast<ASplineActor>(BBComp->GetValueAsObject(SplineActorKey.SelectedKeyName));
		NULLCHECK_CODE_RETURN_LOG(SplineActor, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )
		NULLCHECK_CODE_RETURN_LOG(SplineActor->SplineComponent, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

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
