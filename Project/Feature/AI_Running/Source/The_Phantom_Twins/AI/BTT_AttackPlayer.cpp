#include "BTT_AttackPlayer.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTT_AttackPlayer::UBTT_AttackPlayer()
{
    NodeName = TEXT("Attack Player");
    bNotifyTick = true; // 애니메이션 끝날 때까지 대기
}

EBTNodeResult::Type UBTT_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("ChasingPlayer")));
    if (!Target) return EBTNodeResult::Failed;

    if (!AIController->LineOfSightTo(Target))
    {
        return EBTNodeResult::Failed;
    }

    if (Target->IsGroggy())
    {
        BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
        AIController->ResetStimulus();
        BlackboardComp->ClearValue(TEXT("ChasingPlayer"));
        return EBTNodeResult::Succeeded;
    }

    // 1. 공격 몽타주 재생 (서버에서 Multicast로 동기화)
    AIPawn->PlayJogFwdMontageSynced(1.0f);

    // 2. 데미지 판정 즉시 실행
    UGameplayStatics::ApplyDamage(
        Target,
        DamageAmount,
        AIController,
        AIPawn,
        UDamageType::StaticClass()
    );

    // 3. 애니메이션이 끝날 때까지 대기
    return EBTNodeResult::InProgress;
}

void UBTT_AttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return;

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn) return;

    if (!AIPawn->GetMesh()->GetAnimInstance()->Montage_IsPlaying(AIPawn->JogFwdMontage))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
