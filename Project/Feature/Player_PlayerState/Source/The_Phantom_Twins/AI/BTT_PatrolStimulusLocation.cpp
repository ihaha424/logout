// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolStimulusLocation.h"

#include <NavigationPath.h>

#include "MyAICharacter.h"
#include "MyAIController.h"
#include "SplinePathActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_PatrolStimulusLocation::UBTT_PatrolStimulusLocation()
{
	NodeName = TEXT("Patrol Stimulus Route");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PatrolStimulusLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn || !AIPawn->StimulusSplinePath) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	USplineComponent* SplineRoute = AIPawn->StimulusSplinePath->SplineComponent;
	if (!SplineRoute)
	{
		UE_LOG(LogTemp, Warning, TEXT("!SplineRoute"));
		BlackboardComp->ClearValue("PlayerStimulusLocation");
		BlackboardComp->ClearValue("UpdatedStimulusLocation");
		BlackboardComp->ClearValue("TargetSplinePath");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	int32 CurrentIndex = BlackboardComp->GetValueAsInt(TEXT("StimulusPatrolIndex"));
	AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// 현재 순찰 포인트 위치
	FVector StartLocation = AIPawn->GetActorLocation();
	FVector TargetLocation = SplineRoute->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),                // UWorld*
		StartLocation,             // 시작 위치
		TargetLocation,            // 목표 위치
		AIPawn                     // Navigation Agent (예: AI Pawn)
	);
	if (!NavPath || NavPath->PathPoints.Num() <= 1 || !NavPath->IsValid() || NavPath->IsPartial())
	{
		BlackboardComp->ClearValue("PlayerStimulusLocation");
		BlackboardComp->ClearValue("UpdatedStimulusLocation");
		BlackboardComp->ClearValue("TargetSplinePath");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		AIPawn->StimulusSplinePath = nullptr;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	// MoveTo 실행
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	FNavPathSharedPtr OutPath;
	if (AIController->MoveTo(MoveRequest, &OutPath) == EPathFollowingRequestResult::Failed)
	{
		// 실패했으므로 상태 초기화
		BlackboardComp->ClearValue("PlayerStimulusLocation");
		BlackboardComp->ClearValue("UpdatedStimulusLocation");
		BlackboardComp->ClearValue("TargetSplinePath");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		AIPawn->StimulusSplinePath = nullptr;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsVector(TEXT("PatrolDestination"), TargetLocation);
	return EBTNodeResult::InProgress;
}
void UBTT_PatrolStimulusLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn || !AIPawn->StimulusSplinePath)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    USplineComponent* SplineRoute = AIPawn->StimulusSplinePath->SplineComponent;
    if (!SplineRoute)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 이동 상태 직접 확인
    EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
    if (MoveStatus == EPathFollowingStatus::Idle)
    {
        // 목적지 도착 또는 이동 중단
        int32 CurrentIndex = BlackboardComp->GetValueAsInt(TEXT("StimulusPatrolIndex"));
        const int32 MaxIndex = SplineRoute->GetNumberOfSplinePoints() - 1;

        if (CurrentIndex > MaxIndex)
        {
            // 순찰 종료
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
            BlackboardComp->ClearValue("TargetSplinePath");
            BlackboardComp->SetValueAsInt(TEXT("StimulusPatrolIndex"), 0);
            BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
            AIPawn->StimulusSplinePath = nullptr;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
        else
        {
            // 다음 포인트로 이동
            CurrentIndex++;
            BlackboardComp->SetValueAsInt(TEXT("StimulusPatrolIndex"), CurrentIndex);

            if (CurrentIndex > MaxIndex)
            {
                // 마지막 포인트를 넘어간 경우
                BlackboardComp->ClearValue("PlayerStimulusLocation");
                BlackboardComp->ClearValue("UpdatedStimulusLocation");
                BlackboardComp->ClearValue("TargetSplinePath");
                BlackboardComp->SetValueAsInt(TEXT("StimulusPatrolIndex"), 0);
                BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
                AIPawn->StimulusSplinePath = nullptr;
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
            else
            {
                // 다음 포인트로 이동 재시작
                const FVector TargetLocation = SplineRoute->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
                BlackboardComp->SetValueAsVector(TEXT("PatrolDestination"), TargetLocation);

                FAIMoveRequest MoveRequest;
                MoveRequest.SetGoalLocation(TargetLocation);
                if (AIController->MoveTo(MoveRequest) == EPathFollowingRequestResult::Failed)
                {
                    BlackboardComp->ClearValue("PlayerStimulusLocation");
                    BlackboardComp->ClearValue("UpdatedStimulusLocation");
                    BlackboardComp->ClearValue("TargetSplinePath");
                    BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
                    AIPawn->StimulusSplinePath = nullptr;
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                }
            }
        }
    }
    else if (MoveStatus == EPathFollowingStatus::Waiting || MoveStatus == EPathFollowingStatus::Paused)
    {
        // 이동이 비정상적으로 중단된 경우
        BlackboardComp->ClearValue("PlayerStimulusLocation");
        BlackboardComp->ClearValue("UpdatedStimulusLocation");
        BlackboardComp->ClearValue("TargetSplinePath");
        BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
        AIPawn->StimulusSplinePath = nullptr;
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }
}

