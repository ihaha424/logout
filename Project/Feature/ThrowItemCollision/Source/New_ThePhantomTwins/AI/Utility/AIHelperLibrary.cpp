// Fill out your copyright notice in the Description page of Project Settings.


#include "AIHelperLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIBaseState.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

void UAIHelperLibrary::SetAIStateAndTag(UBehaviorTreeComponent* OwnerComp, UBlackboardComponent* BB, EAIBaseState CurState, EAIBaseState NextState, FName AIStateKeyName)
{
    if (CurState == NextState)
        return;

    if (!OwnerComp || !BB)
        return;

    AActor* Actor = OwnerComp->GetOwner();
    if (!Actor)
        return;

    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
    if (!ASC)
        return;

    FGameplayTag curTag = GetTagFromAIState(CurState);
    if (curTag.IsValid())
    {
        ASC->RemoveLooseGameplayTag(curTag);
        ASC->RemoveReplicatedLooseGameplayTag(curTag);
    }

    FGameplayTag nextTag = GetTagFromAIState(NextState);
    if (nextTag.IsValid())
    {
        ASC->AddLooseGameplayTag(nextTag);
        ASC->AddReplicatedLooseGameplayTag(nextTag);
    }

    BB->SetValueAsEnum(AIStateKeyName, static_cast<uint8>(NextState));
}

FGameplayTag UAIHelperLibrary::GetTagFromAIState(EAIBaseState State)
{
    switch (State)
    {
    case EAIBaseState::Default:
        return FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default;
    case EAIBaseState::Suspicion:
        return FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Suspicion;
    case EAIBaseState::Combat:
        return FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat;
    case EAIBaseState::Stun:
        return FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Stun;
    case EAIBaseState::Die:
        return FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Die;
    default:
        return FGameplayTag();
    }
}
