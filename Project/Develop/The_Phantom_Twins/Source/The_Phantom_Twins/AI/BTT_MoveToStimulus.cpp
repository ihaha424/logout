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

    FVector CurrentLocation = AIPawn->GetNavAgentLocation();
    FVector TargetLocation = BlackboardComp->GetValueAsVector(TEXT("UpdatedStimulusLocation"));

    CurrentLocation.Z = 0;
    TargetLocation.Z = 0;
    float InitialDistance = FVector::Dist(CurrentLocation, TargetLocation);

    // 이미 도착한 경우
    if (InitialDistance <= AcceptanceRadius)
    {
        BlackboardComp->ClearValue("PlayerStimulusLocation");
        BlackboardComp->ClearValue("UpdatedStimulusLocation");
        BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
        return EBTNodeResult::Failed;
    }

    AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // 네비게이션 경로 찾기 (Partial Path도 허용)
    UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
        GetWorld(),
        CurrentLocation,
        TargetLocation,
        AIPawn
    );

    if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
    {
        // Partial Path여도 마지막 PathPoint까지 이동 시도
        FVector MoveGoal = NavPath->PathPoints.Last();
        MyMemory->TargetLocation = MoveGoal;
        MyMemory->bPathValid = true;
        // 어디정도까지 도착하면 인정해줄껀지 가라고 하는거
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(MoveGoal);
        MoveRequest.SetAcceptanceRadius(TryRadius);

        EPathFollowingRequestResult::Type MoveResult = MyAIController->MoveTo(MoveRequest);

        if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
        {
            DrawDebugSphere(AIPawn->GetWorld(), MoveGoal, 20.f, 12, FColor::Green, false, 10.0f);
            return EBTNodeResult::InProgress;
        }
        else
        {
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
            BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        // 경로 자체가 없으면 실패
        DrawDebugSphere(AIPawn->GetWorld(), TargetLocation, 25.0f, 12, FColor::Red, false, 10.0f);
        BlackboardComp->ClearValue("PlayerStimulusLocation");
        BlackboardComp->ClearValue("UpdatedStimulusLocation");
        BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
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

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(MyAIController->GetPawn());
    if (!AIPawn) return;

    UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
    if (!BlackboardComp) return;

    FVector MyLocation = AIPawn->GetNavAgentLocation();
    FVector TargetLocation = MyMemory->TargetLocation;
    TargetLocation.Z = 0;
    MyLocation.Z = 0;
    float DistanceToGoal = FVector::Dist(MyLocation, TargetLocation);

    EPathFollowingStatus::Type MoveStatus = MyAIController->GetMoveStatus();

    // Idle(이동 완료) 상태에서 목표 지점(Partial Path의 마지막 지점)에 도달했는지 확인
    if (MoveStatus == EPathFollowingStatus::Idle)
    {
        if (DistanceToGoal <= AcceptanceRadius)
        {
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
           // UE_LOG(LogTemp, Error, TEXT("rkddlsrbrpdl Succ"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
        else
        {
            // 목표 지점에 도달하지 못했다면 실패 처리
            BlackboardComp->ClearValue("PlayerStimulusLocation");
            BlackboardComp->ClearValue("UpdatedStimulusLocation");
           // UE_LOG(LogTemp, Error, TEXT("rkddlsrbrpdl Fail: %f"), DistanceToGoal);
            BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        }
    }
}

void UBTT_MoveToStimulus::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    AMyAIController* MyAIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (MyAIController)
    {
        MyAIController->StopMovement();

        if (TaskResult == EBTNodeResult::Failed)
        {
            UBlackboardComponent* BlackboardComp = MyAIController->GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(EMyAIState::Default));
            }
        }
    }
}
