// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_GASBaseTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "Log/TPTLog.h"

UBTT_GASBaseTask::UBTT_GASBaseTask() 
{
    TagCheckPhase = EGameplayTagCheckPhase::None;
    BlockTagMode = EGASTagCheckMode::None;
    RequiredTagMode = EGASTagCheckMode::None;
}

EBTNodeResult::Type UBTT_GASBaseTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (TagCheckPhase == EGameplayTagCheckPhase::OnExecuteOnly || TagCheckPhase == EGameplayTagCheckPhase::ExecuteAndTick)
    {
        AAIController* AIController = OwnerComp.GetAIOwner();
        APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;
        NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, EBTNodeResult::Failed;)

        UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
        NULLCHECK_RETURN_LOG(ASC, AILog, Warning, EBTNodeResult::Failed;)

        const EGASTagCheckMode TagResult = EvaluateTagBlocking(ASC);
        switch (TagResult)
        {
        case EGASTagCheckMode::Fail:
            return EBTNodeResult::Failed;
            break;
        case EGASTagCheckMode::Succeed:
            return EBTNodeResult::Succeeded;
            break;
        case EGASTagCheckMode::Wait:
            break;
        case EGASTagCheckMode::Abort:
            return EBTNodeResult::Aborted;
            break;
        case EGASTagCheckMode::Exception:
            return Execute_TaskException(OwnerComp, NodeMemory);
            break;
        default:
            break;
        }
    }
    return Execute_Task(OwnerComp, NodeMemory);
}

void UBTT_GASBaseTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (TagCheckPhase == EGameplayTagCheckPhase::OnTickOnly || TagCheckPhase == EGameplayTagCheckPhase::ExecuteAndTick)
    {
        AAIController* AIController = OwnerComp.GetAIOwner();
        APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;
        NULLCHECK_CODE_RETURN_LOG(AIPawn, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

        UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
        NULLCHECK_CODE_RETURN_LOG(ASC, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

        const EGASTagCheckMode TagResult = EvaluateTagBlocking(ASC);
        switch (TagResult)
        {
        case EGASTagCheckMode::Fail:
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
            return;
        case EGASTagCheckMode::Succeed:
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        case EGASTagCheckMode::Wait:
            return;
            break;
        case EGASTagCheckMode::Abort:
            FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
            return;
        case EGASTagCheckMode::Exception:
            Execute_TickTaskException(OwnerComp, NodeMemory, DeltaSeconds);
            return;
        default:
            break;
        }
    }
    Execute_TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EGASTagCheckMode UBTT_GASBaseTask::EvaluateTagBlocking(UAbilitySystemComponent* ASC)
{
    auto GetPriority = [](EGASTagCheckMode Mode)
        {
            switch (Mode)
            {
            case EGASTagCheckMode::Exception:   return 5;
            case EGASTagCheckMode::Abort:       return 4;
            case EGASTagCheckMode::Fail:        return 3;
            case EGASTagCheckMode::Wait:        return 2;
            case EGASTagCheckMode::Succeed:     return 1;
            default:                            return 0;
            }
        };

    EGASTagCheckMode BlockResult = EGASTagCheckMode::None;
    if (BlockTagMode != EGASTagCheckMode::None)
    {
        const bool bHasBlock = ASC->HasAnyMatchingGameplayTags(FGameplayTagContainer::CreateFromArray(BlockTags));
        if (bHasBlock) BlockResult = BlockTagMode;
    }

    EGASTagCheckMode RequiredResult = EGASTagCheckMode::None;
    if (RequiredTagMode != EGASTagCheckMode::None)
    {
        const bool bMissingRequired = !ASC->HasAllMatchingGameplayTags(FGameplayTagContainer::CreateFromArray(RequiredTags));
        if (bMissingRequired) RequiredResult = RequiredTagMode;
    }

    // 우선순위 비교
    return GetPriority(BlockResult) >= GetPriority(RequiredResult) ? BlockResult : RequiredResult;
}
