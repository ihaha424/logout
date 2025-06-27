#include "BTT_AIIsHacked.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UBTT_AIIsHacked::UBTT_AIIsHacked()
{
    NodeName = TEXT("AI Is Hacked");
}

EBTNodeResult::Type UBTT_AIIsHacked::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    AMyAICharacter* AICharacter = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        return EBTNodeResult::Failed;
    }

    // 1. 이동 멈추기
    AICharacter->GetCharacterMovement()->StopMovementImmediately();
    AIController->StopMovement();

    // 2. JumpMontage 재생 (서버에서 Multicast로 동기화)
    AICharacter->PlayJumpMontageSynced(1.0f);

    // 3. 5초 후 Task 완료
    FTimerDelegate TimerDel;
    TimerDel.BindLambda([this, &OwnerComp]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

    FTimerHandle TimerHandle;
    AIController->GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        TimerDel,
        5.0f,
        false
    );

    return EBTNodeResult::InProgress;
}
