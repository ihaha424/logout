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
	UE_LOG(LogTemp, Warning, TEXT("1"));

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	UE_LOG(LogTemp, Warning, TEXT("2"));

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;
	UE_LOG(LogTemp, Warning, TEXT("3"));

	AMyAICharacter* MyAICharacter = Cast<AMyAICharacter>(AIPawn);
	if (!MyAICharacter || !MyAICharacter->SplinePath) return EBTNodeResult::Failed;
	UE_LOG(LogTemp, Warning, TEXT("4"));
	USplineComponent* SplineRoute = MyAICharacter->SplinePath->SplineComponent;
	if (!SplineRoute) return EBTNodeResult::Failed;
	UE_LOG(LogTemp, Warning, TEXT("5"));
	MaxIndex = SplineRoute->GetNumberOfSplinePoints();
	FVector PatrolPoint = SplineRoute->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
	UE_LOG(LogTemp, Warning, TEXT("Dist: %f"), FVector::Dist(MyAICharacter->GetActorLocation(), PatrolPoint));
	UE_LOG(LogTemp, Warning, TEXT("ActorLocation: %s, PatrolPoint: %s, Index: %d"), *MyAICharacter->GetActorLocation().ToString(), *PatrolPoint.ToString(), Index);
	if (FVector::Dist(MyAICharacter->GetActorLocation(), PatrolPoint) < 150.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("6"));
		Index = (1 + Index) % MaxIndex;
		FVector NextPoint = SplineRoute->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
		BlackboardComp->SetValueAsVector(TEXT("PatrolLocation"), NextPoint);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("7"));
		BlackboardComp->SetValueAsVector(TEXT("PatrolLocation"), PatrolPoint);
	}

	return EBTNodeResult::Succeeded;
}