// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ObjectRestoration.h"
#include "MyAIController.h"
#include "MyAICharacter.h"
#include <SzObjects/HackableObject.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

UBTT_ObjectRestoration::UBTT_ObjectRestoration()
{
	NodeName = TEXT("Turn Off Object");
}

EBTNodeResult::Type UBTT_ObjectRestoration::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	AHackableObject* Target = Cast<AHackableObject>(BlackboardComp->GetValueAsObject(TEXT("TargetObject")));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}
	float Distance = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());

	if (Distance < 100)
	{
		Target->ClearHacking_Implementation();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}
