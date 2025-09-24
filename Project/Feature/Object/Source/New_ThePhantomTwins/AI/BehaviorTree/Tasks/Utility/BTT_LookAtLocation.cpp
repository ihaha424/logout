// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAtLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Log/TPTLog.h"

UBTT_LookAtLocation::UBTT_LookAtLocation()
{
	NodeName = TEXT("LookAtLocation");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_LookAtLocation::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, EBTNodeResult::Failed);
	UObject* LookAtObject = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);

	if (nullptr == LookAtObject)
		return EBTNodeResult::Failed;
	else
		return EBTNodeResult::InProgress;
}

void UBTT_LookAtLocation::Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_CODE_RETURN_LOG(AIController, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );
	APawn* AIPawn = AIController->GetPawn();
	NULLCHECK_CODE_RETURN_LOG(AIPawn, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_CODE_RETURN_LOG(BB, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, );
	UObject* LookAtObject = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AActor* LookAtActor = Cast<AActor>(LookAtObject);

	if (nullptr != LookAtActor)
	{
		FRotator CurrentRot = AIPawn->GetActorRotation();
		FRotator TargetRot = (LookAtActor->GetActorLocation() - AIPawn->GetActorLocation()).Rotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, InterpSpeed);

		AIPawn->SetActorRotation(NewRot);
	}
	else
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

FString UBTT_LookAtLocation::GetStaticDescription() const
{
	return FString("Look at the target Actor.");
}
