#include "BTT_MoveToStimulus.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTT_MoveToStimulus::UBTT_MoveToStimulus()
{
	NodeName = TEXT("Move To Stimulus Location");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_MoveToStimulus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	FBTMoveToStimulusMemory* MyMemory = (FBTMoveToStimulusMemory*)NodeMemory;

	AMyAIController* MyAIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!MyAIController) return EBTNodeResult::Failed;

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(MyAIController->GetPawn());
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	FVector StartLocation = AIPawn->GetActorLocation();
	FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("UpdatedStimulusLocation"));

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		StartLocation,
		TargetLocation,
		AIPawn
		);
	
	// 경로 유효시 이동 시작
	if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1 && !NavPath->IsPartial())
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(TargetLocation);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		MyAIController->MoveTo(MoveRequest);

		UE_LOG(LogTemp, Warning, TEXT("AI CAN move TO UpdatedStimulusLocation"));
		
		MyMemory->TargetLocation = TargetLocation;
		MyMemory->bPathValid = true;

		DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 20.f, 12, FColor::Green, false, 10.0f);

		return EBTNodeResult::InProgress;
	}
	else
	{
		DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 25.0f, 12, FColor::Red, false, 10.0f);
		UE_LOG(LogTemp, Warning, TEXT("AI Move TO UpdatedStimulusLocation IS FAILED. CAN'T GO"));
		BlackboardComp->ClearValue("PlayerStimulusLocation");
		BlackboardComp->ClearValue("UpdatedStimulusLocation");
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));

		return EBTNodeResult::Failed;
	}
}

void UBTT_MoveToStimulus::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTMoveToStimulusMemory* MyMemory = (FBTMoveToStimulusMemory*)NodeMemory;
	if (!MyMemory->bPathValid)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	float Distance = FVector::Dist(AIPawn->GetActorLocation(), MyMemory->TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("AI IS Moving TO UpdatedStimulusLocation"));
	if (Distance <= AcceptanceRadius)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_MoveToStimulus::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}
}
