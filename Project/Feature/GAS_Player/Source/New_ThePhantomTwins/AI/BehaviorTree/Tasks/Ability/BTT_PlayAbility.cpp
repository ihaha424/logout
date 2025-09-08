// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayAbility.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Data/NamePayload.h"
#include "../../../Character/AIBaseCharacter.h"
#include "Log/TPTLog.h"


UBTT_PlayAbility::UBTT_PlayAbility()
{
	NodeName = TEXT("Play GAS Ability");
    AbilityTag = FGameplayTag();
    bHasTarget = false;
    TargetActorKey = FBlackboardKeySelector();
    bIsWaitingForAbility = false;
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PlayAbility::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon ? AICon->GetPawn() : nullptr);
    if (!AICon || !AIPawn) return EBTNodeResult::Failed;

    UAbilitySystemComponent* ASC = AIPawn->GetAbilitySystemComponent();
    if (!ASC) return EBTNodeResult::Failed;

    AActor* TargetActor = nullptr;
    if (bHasTarget && TargetActorKey.SelectedKeyName.IsValid())
    {
        UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
        TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    }

    FGameplayEventData EventData;
    EventData.Instigator = AIPawn;
    EventData.Target = TargetActor;
    if (!StringData.IsNone())
    {
        UNamePayload* Payload = NewObject<UNamePayload>();
        Payload->Name = StringData;
        EventData.OptionalObject = Payload;
    }

    ASC->HandleGameplayEvent(AbilityTag, &EventData);

    bIsWaitingForAbility = true;
    return EBTNodeResult::InProgress;
}

void UBTT_PlayAbility::Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!bIsWaitingForAbility)
        return;
    AAIController* AICon = OwnerComp.GetAIOwner();
    AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon ? AICon->GetPawn() : nullptr);
    NULLCHECK_CODE_RETURN_LOG(AICon, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )
    NULLCHECK_CODE_RETURN_LOG(AIPawn, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )

    UAbilitySystemComponent* ASC = AIPawn->GetAbilitySystemComponent();
    NULLCHECK_CODE_RETURN_LOG(ASC, AILog, Warning, FinishLatentTask(OwnerComp, EBTNodeResult::Failed);, )


    if (!ASC->HasMatchingGameplayTag(AbilityTag))
    {
        bIsWaitingForAbility = false;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

FString UBTT_PlayAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Tag : %s | Target: Set BlackBoard Key"), *AbilityTag.ToString());
}
