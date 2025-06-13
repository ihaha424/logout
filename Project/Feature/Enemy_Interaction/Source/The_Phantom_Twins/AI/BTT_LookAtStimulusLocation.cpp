// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAtStimulusLocation.h"

#include "AIInterface.h"
#include "MyAIController.h"
#include "MyAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_LookAtStimulusLocation::UBTT_LookAtStimulusLocation()
{
	NodeName = TEXT("Look At Stimulus Location");
}

EBTNodeResult::Type UBTT_LookAtStimulusLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("LastStimulusLocation"));
	FVector Direction = TargetLocation - AIPawn->GetActorLocation();
	Direction.Z = 0.f; // 수평 회전만

	if (!Direction.IsNearlyZero())
	{
		FRotator LookRotation = Direction.Rotation();
		AIPawn->SetActorRotation(LookRotation);
	}
	return EBTNodeResult::Succeeded;
}
