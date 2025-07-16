// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayAbility.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../../../Character/AIBaseCharacter.h"
#include "GameplayTagContainer.h"

UBTT_PlayAbility::UBTT_PlayAbility()
{
	NodeName = TEXT("Play GAS Ability");
}

EBTNodeResult::Type UBTT_PlayAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    ASC->HandleGameplayEvent(AbilityTag, &EventData);

    return EBTNodeResult::Succeeded;
}

FString UBTT_PlayAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Tag : %s | Target: Set BlackBoard Key"), *AbilityTag.ToString());
}
