// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChaseTarget.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Log/TPTLog.h"

struct FChaseTargetTaskData
{
	FAIRequestID MoveId = FAIRequestID();
};

UBTT_ChaseTarget::UBTT_ChaseTarget()
{
	NodeName = "ChaseTarget";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_ChaseTarget::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, EBTNodeResult::Failed);

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActor.SelectedKeyName));
	NULLCHECK_RETURN_LOG(Target, AILog, Warning, EBTNodeResult::Failed);

	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AIController, AILog, Warning, EBTNodeResult::Failed);

	APawn* AIPawn = AIController->GetPawn();
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, EBTNodeResult::Failed);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Target->GetActorLocation());
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);
	CurTick = 0.f;

	FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
	switch (MoveResult.Code)
	{
	case EPathFollowingRequestResult::RequestSuccessful:
		return EBTNodeResult::InProgress;

	case EPathFollowingRequestResult::AlreadyAtGoal:
			return EBTNodeResult::InProgress;
	case EPathFollowingRequestResult::Failed:
		TPT_LOG(AILog, Warning, TEXT("EPathFollowingRequestResult: Failed."));
	default:
		return EBTNodeResult::Failed;
	}
}

void UBTT_ChaseTarget::Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FChaseTargetTaskData* TaskData = (FChaseTargetTaskData*)NodeMemory;
	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_CODE_RETURN_LOG(AIController, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );
	EPathFollowingStatus::Type Status = AIController->GetPathFollowingComponent()->GetStatus();
	if (Status == EPathFollowingStatus::Idle 
		|| Status == EPathFollowingStatus::Waiting 
		|| (Status == EPathFollowingStatus::Moving && CurTick >= ChaseTick))
	{
		CurTick = 0;
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

		AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActor.SelectedKeyName));
		NULLCHECK_RETURN_LOG(Target, AILog, Warning, );

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(Target->GetActorLocation());
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);

		FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
		TaskData->MoveId = MoveResult.MoveId;
		if (MoveResult.Code == EPathFollowingRequestResult::Failed)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else if (Status == EPathFollowingStatus::Paused)
	{
		AIController->ResumeMove(TaskData->MoveId);
	}
	else
	{
		CurTick += DeltaSeconds;
	}
}

void UBTT_ChaseTarget::Execute_TickTaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FChaseTargetTaskData* TaskData = (FChaseTargetTaskData*)NodeMemory;
	NULLCHECK_CODE_RETURN_LOG(TaskData, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );

	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_CODE_RETURN_LOG(AIController, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );

	EPathFollowingStatus::Type Status = AIController->GetPathFollowingComponent()->GetStatus();
	if (Status == EPathFollowingStatus::Moving)
	{
		AIController->PauseMove(TaskData->MoveId);
	}
}

EBTNodeResult::Type UBTT_ChaseTarget::Execute_TaskException(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTT_ChaseTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (TaskResult == EBTNodeResult::Aborted)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		NULLCHECK_CODE_RETURN_LOG(AIController, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );

		AIController->StopMovement();
	}
}

uint16 UBTT_ChaseTarget::GetInstanceMemorySize() const
{
	return sizeof(FChaseTargetTaskData);
}

FString UBTT_ChaseTarget::GetStaticDescription() const
{
	return FString("Keep following the target to its position.");
}
