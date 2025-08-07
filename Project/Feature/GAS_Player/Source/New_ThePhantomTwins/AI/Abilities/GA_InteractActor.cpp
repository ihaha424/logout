// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InteractActor.h"
#include "SzInterface/Interact.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_InteractActor::UGA_InteractActor()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_InteractActor);
    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_InteractActor;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_InteractActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC)
    {
        FGameplayEventData EventData;
        EventData.Instigator = TriggerEventData->Instigator.Get();
        EventData.Target = TriggerEventData->Target.Get();

        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat))
            ASC->HandleGameplayEvent(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle, TriggerEventData);
        else
            ASC->HandleGameplayEvent(FTPTGameplayTags::Get().TPTGameplay_Character_Action_OpenObstacle, TriggerEventData);
    }
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_InteractActor::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
