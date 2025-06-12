// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolStimulusLocation.h"

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
	if (!SplineRoute) return EBTNodeResult::Failed;

	MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	CurrentIndex = BlackboardComp->GetValueAsInt(TEXT("CurrentPatrolIndex"));

	if (ACharacter* AICharacter = Cast<ACharacter>(AIPawn))
	{
		AICharacter->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}

	// 현재 순찰 포인트 위치
	TargetLocation = SplineRoute->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);

	// MoveTo 실행
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	FNavPathSharedPtr OutPath;
	if (AIController->MoveTo(MoveRequest, &OutPath) == EPathFollowingRequestResult::Failed)
	{
		// 실패했으므로 상태 초기화
		BlackboardComp->ClearValue("LastStimulusLocation");
		BlackboardComp->ClearValue("PatrolStimulusLocation");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));

		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsVector(TEXT("PatrolStimulusLocation"), TargetLocation);
	return EBTNodeResult::InProgress;
}
void UBTT_PatrolStimulusLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetLocation);
	if (Distance <= 150.f)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (!BlackboardComp) return;

		// 다음 포인트로 인덱스 증가
		CurrentIndex++;
		if (CurrentIndex > MaxIndex)
		{
			// 순찰 종료
			BlackboardComp->ClearValue("LastStimulusLocation");
			BlackboardComp->ClearValue("PatrolStimulusLocation");
			BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		}
		else
		{
			// 다음 포인트 저장
			BlackboardComp->SetValueAsInt(TEXT("CurrentPatrolIndex"), CurrentIndex);
		}

		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}