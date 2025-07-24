// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindClosestSplinePath.h"
#include "AIController.h"
#include "AI/Utility/SplineActorManager.h"
#include "AI/Utility/SplineActor.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "Log/TPTLog.h"

UBTT_FindClosestSplinePath::UBTT_FindClosestSplinePath()
{
	NodeName = "FindClosestSplinePath";
}

EBTNodeResult::Type UBTT_FindClosestSplinePath::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AIController, AILog, Warning, EBTNodeResult::Failed)

	APawn* AIPawn = AIController->GetPawn();
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, EBTNodeResult::Failed)

	ASplineActorManager* Manager = ASplineActorManager::Get(GetWorld());
	NULLCHECK_RETURN_LOG(Manager, AILog, Warning, EBTNodeResult::Failed)

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BBComp, AILog, Warning, EBTNodeResult::Failed)

	FVector ActorLocation = AIPawn->GetActorLocation();
	FVector2D ActorLocation2D = FVector2D(ActorLocation.X, ActorLocation.Y);
	ASplineActor* SplineActor = Manager->FindNearestSpline(ActorLocation2D);

	BBComp->SetValueAsObject(SplineActorKey.SelectedKeyName, SplineActor);

	return EBTNodeResult::Succeeded;
}

FString UBTT_FindClosestSplinePath::GetStaticDescription() const
{
	return FString("FindClosestSplinePath");
}
