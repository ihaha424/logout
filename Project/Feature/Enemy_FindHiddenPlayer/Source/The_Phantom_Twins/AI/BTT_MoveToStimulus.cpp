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

    FVector CurrentLocation = AIPawn->GetActorLocation();
    FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("UpdatedStimulusLocation"));
    float InitialDistance = FVector::Dist(CurrentLocation, TargetLocation);

    // 이미 도착한 경우 실패 반환 및 상태 초기화
    if (InitialDistance <= AcceptanceRadius)
    {
        UE_LOG(LogTemp, Warning, TEXT("Move To Stimulus Location : Already at target location! : %s"), *AIPawn->GetName());
        BlackboardComp->ClearValue("PlayerStimulusLocation");
        BlackboardComp->ClearValue("UpdatedStimulusLocation");
        BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
        return EBTNodeResult::Failed;
    }

    AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
        GetWorld(),
        CurrentLocation,
        TargetLocation,
        AIPawn
    );

    if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1 && !NavPath->IsPartial())
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(TargetLocation);
        MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

        // 이동 명령 결과 확인
        EPathFollowingRequestResult::Type MoveResult = MyAIController->MoveTo(MoveRequest);

        if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
        {
            UE_LOG(LogTemp, Warning, TEXT("Move To Stimulus Location : Movement started successfully : %s"), *AIPawn->GetName());
            MyMemory->TargetLocation = TargetLocation;
            MyMemory->bPathValid = true;
            DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 20.f, 12, FColor::Green, false, 10.0f);
            return EBTNodeResult::InProgress;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Move To Stimulus Location : Failed to start movement! Reason: %d"), (int32)MoveResult);
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
            BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 25.0f, 12, FColor::Red, false, 10.0f);
        UE_LOG(LogTemp, Warning, TEXT("AI Move TO UpdatedStimulusLocation IS FAILED. CAN'T GO : %s"), *AIPawn->GetName());
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

    AMyAIController* MyAIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!MyAIController) return;

    APawn* AIPawn = MyAIController->GetPawn();
    if (!AIPawn) return;

    // 이동 상태 직접 확인
    if (MyAIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
        }
        UE_LOG(LogTemp, Warning, TEXT("AI Movement completed successfully! : %s"), *AIPawn->GetName());
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

void UBTT_MoveToStimulus::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    AMyAIController* MyAIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (MyAIController)
    {
        MyAIController->StopMovement();

        // 실패한 경우에만 상태 변경
        if (TaskResult == EBTNodeResult::Failed)
        {
            UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
            }
        }
    }
}
