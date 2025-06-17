// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToStimulus.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MoveToStimulus::UBTT_MoveToStimulus()
{
	NodeName = TEXT("Move To Stimulus Location");
	bNotifyTick = true;

}

EBTNodeResult::Type UBTT_MoveToStimulus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AMyAIController* MyAIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!MyAIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(MyAIController->GetPawn());
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	// 추격 속도 적용
	AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	FVector StartLocation = AIPawn->GetActorLocation();
	TargetLocation = BlackboardComp->GetValueAsVector(TEXT("UsingStimulusLocation"));

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),                // UWorld*
		StartLocation,             // 시작 위치
		TargetLocation,            // 목표 위치
		AIPawn                     // Navigation Agent (예: AI Pawn)
	);
	if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
	{
		// 유효한 경로가 있으므로 MoveTo 실행
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(TargetLocation);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		FNavPathSharedPtr DummyPath;
		MyAIController->MoveTo(MoveRequest, &DummyPath);

		bPathValid = true;

		DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 20.f, 12, FColor::Green, false, 10.0f);

		return EBTNodeResult::InProgress;
	}
	else
	{
		// 경로가 유효하지 않으면 실패 처리
		DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 25.0f, 12, FColor::Red, false, 10.0f);

		BlackboardComp->ClearValue("LastStimulusLocation");
		BlackboardComp->ClearValue("UsingStimulusLocation");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		return EBTNodeResult::Failed;
	}
}


void UBTT_MoveToStimulus::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bPathValid)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetLocation);

	if (Distance <= AcceptanceRadius)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}