// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTree/Services/BTS_AddTagWhileActive.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include "Log/TPTLog.h"

UBTS_AddTagWhileActive::UBTS_AddTagWhileActive()
{
	NodeName = TEXT("AddTagWhileActive");

	bNotifyTick = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTS_AddTagWhileActive::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AICon, AILog, Warning,);
	APawn* AIPawn = Cast<APawn>(AICon->GetPawn());
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, );
	UAbilitySystemComponent* AIASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
	NULLCHECK_RETURN_LOG(AIASC, AILog, Warning, );

	if (Tag.IsValid())
	{
		AIASC->AddLooseGameplayTag(Tag);
		AIASC->AddReplicatedLooseGameplayTag(Tag);
	}
}

void UBTS_AddTagWhileActive::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AICon, AILog, Warning, );
	APawn* AIPawn = Cast<APawn>(AICon->GetPawn());
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, );
	UAbilitySystemComponent* AIASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIPawn);
	NULLCHECK_RETURN_LOG(AIASC, AILog, Warning, );

	if (Tag.IsValid())
	{
		AIASC->RemoveLooseGameplayTag(Tag);
		AIASC->RemoveReplicatedLooseGameplayTag(Tag);
	}
}

FString UBTS_AddTagWhileActive::GetStaticDescription() const
{
	return FString::Printf(TEXT("A service that tags when a subtree reduces execution and removes it when finished."));
}
