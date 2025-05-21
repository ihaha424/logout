// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PatrolSplineRoute.h"
#include "Components/SplineComponent.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "SplinePathActor.h"
#include "AIController.h"
#include "MyAICharacter.h"

UBTT_PatrolSplineRoute::UBTT_PatrolSplineRoute()
{
	NodeName = TEXT("Patrol Spline Route");
}

EBTNodeResult::Type UBTT_PatrolSplineRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	AMyAICharacter* MyAICharacter = Cast<AMyAICharacter>(AIPawn);
	if (!MyAICharacter || !MyAICharacter->SplinePath) return EBTNodeResult::Failed;

	USplineComponent* SplineRoute = MyAICharacter->SplinePath->SplineComponent;
	if (!SplineRoute) return EBTNodeResult::Failed;

	MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	FVector PatrolPoint = SplineRoute->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
	if (FVector::Dist(MyAICharacter->GetActorLocation(), PatrolPoint) < 150.f)
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