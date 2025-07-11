// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolSplineRoute.h"

#include <Navigation/PathFollowingComponent.h>

#include "Components/SplineComponent.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "SplinePathActor.h"
#include "AIController.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTT_PatrolSplineRoute::UBTT_PatrolSplineRoute()
{
	NodeName = TEXT("Patrol Spline Route");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PatrolSplineRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn || !AIPawn->BaseSplinePath)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	USplineComponent* SplineRoute = AIPawn->BaseSplinePath->SplineComponent;
	if (!SplineRoute)
	{
		return EBTNodeResult::Failed;
	}

	AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	BlackboardComp->SetValueAsInt(TEXT("MaxPatrolIndex"), MaxIndex);
	int32 CurrentIndex = BlackboardComp->GetValueAsInt(TEXT("CurrentPatrolIndex"));

	// 목표 위치 설정
	const FVector TargetLocation = SplineRoute->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
	BlackboardComp->SetValueAsVector(TEXT("PatrolDestination"), TargetLocation);

	// 이동 시작
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(50.f); // 도달 반경

	FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);

	return (MoveResult == EPathFollowingRequestResult::RequestSuccessful || MoveResult == EPathFollowingRequestResult::AlreadyAtGoal) ?
		EBTNodeResult::InProgress :
		EBTNodeResult::Failed;
}
void UBTT_PatrolSplineRoute::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return;

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn || !AIPawn->BaseSplinePath) return;

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	// 현재 목표 위치와의 거리 확인
	const FVector CurrentLocation = AIPawn->GetActorLocation();
	const FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("PatrolDestination"));
	const float Distance = FVector::Distance(CurrentLocation, TargetLocation);

	if (Distance <= 150.f) // 도달 반경
	{
		// 다음 인덱스 계산 (순환)
		int32 CurrentIndex = BlackboardComp->GetValueAsInt(TEXT("CurrentPatrolIndex"));
		int32 MaxPatrolIndex = BlackboardComp->GetValueAsInt(TEXT("MaxPatrolIndex"));
		CurrentIndex = (CurrentIndex + 1) % MaxPatrolIndex;
		BlackboardComp->SetValueAsInt(TEXT("CurrentPatrolIndex"), CurrentIndex);

		// 새로운 목표지점 설정 및 이동 재시작
		const FVector NewTarget = AIPawn->BaseSplinePath->SplineComponent->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);
		BlackboardComp->SetValueAsVector(TEXT("PatrolDestination"), NewTarget);

		FAIMoveRequest NewMoveRequest;
		NewMoveRequest.SetGoalLocation(NewTarget);
		AIController->MoveTo(NewMoveRequest);
	}
}