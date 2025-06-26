#include "BTT_ChasePlayer.h"

#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTT_ChasePlayer::UBTT_ChasePlayer()
{
    NodeName = TEXT("Chase Player");
    bNotifyTick = true; // TickTask 사용
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
    APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    // 추격 속도 적용
    AIPawn->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // 플레이어 위치를 네비메쉬 위로 보정
    FVector TargetLocation = Target->GetActorLocation();
    FNavLocation NavLocation;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    bool bFoundNavLoc = false;
    if (NavSys)
    {
        bFoundNavLoc = NavSys->ProjectPointToNavigation(
            TargetLocation,
            NavLocation,
            FVector(500, 500, 500) // 탐색 범위
        );
    }
    FVector MoveGoal = bFoundNavLoc ? NavLocation.Location : TargetLocation;

    // 목표 위치를 블랙보드에 저장해 TickTask에서 참조할 수 있게 함(선택 사항)
    BlackboardComp->SetValueAsVector(TEXT("UpdatedStimulusLocation"), MoveGoal);

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(MoveGoal);
    MoveRequest.SetAcceptanceRadius(150.0f); // 원하는 반경

    FNavPathSharedPtr NavPath;
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveTo(MoveRequest, &NavPath);

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UBTT_ChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 네비메쉬 위 목표 위치를 가져온다
    FVector MoveGoal = BlackboardComp->GetValueAsVector(TEXT("UpdatedStimulusLocation"));
    FVector MyLocation = AIPawn->GetActorLocation();
    float Distance = FVector::Dist(MyLocation, MoveGoal);

    EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();

    // 목표(네비메쉬 위 경계선) 반경 내에 들어오면 성공
    if (Distance <= 200.0f)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    // 이동이 끝났는데(Idle) 목표에 도달하지 못했으면 실패 (최대한 이동한 것)
    if (MoveStatus == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
}
