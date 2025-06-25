// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayAttackMontage.h"
#include "MyAICharacter.h"
#include "MyAIController.h"

UBTT_PlayAttackMontage::UBTT_PlayAttackMontage()
{
    NodeName = TEXT("Play Jog_Fwd Montage Synced");
}

EBTNodeResult::Type UBTT_PlayAttackMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;

    AMyAICharacter* AIChar = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIChar) return EBTNodeResult::Failed;

    // 서버에서만 Multicast로 실행
    AIChar->PlayJogFwdMontageSynced(1.0f); // 1.0f: 재생 속도

    // 즉시 성공 처리 (애니메이션 길이만큼 대기하려면 InProgress + 타이머 사용)
    return EBTNodeResult::Succeeded;
}
