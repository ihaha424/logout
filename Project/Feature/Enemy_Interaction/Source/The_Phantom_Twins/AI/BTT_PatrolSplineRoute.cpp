// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolSplineRoute.h"
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

	MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	FVector PatrolPoint = SplineRoute->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);

	if (FVector::Dist(AIPawn->GetActorLocation(), PatrolPoint) < 150.f)
	{
		Index = (1 + Index) % MaxIndex;
		FVector NextPoint = SplineRoute->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
		BlackboardComp->SetValueAsVector(TEXT("PatrolLocation"), NextPoint);
	}
	else
	{
		BlackboardComp->SetValueAsVector(TEXT("PatrolLocation"), PatrolPoint);
	}

	return EBTNodeResult::Succeeded;
}