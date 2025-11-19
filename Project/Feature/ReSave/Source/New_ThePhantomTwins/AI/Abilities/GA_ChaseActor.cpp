// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ChaseActor.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_ChaseActor::UGA_ChaseActor()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing);

    CancelAbilitiesWithTag.AddTag(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing);
    ActivationRequiredTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_ChaseActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    AActor* TargetActor = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;
    if (nullptr == TargetActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (nullptr == TargetASC)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
  
    FGameplayEffectContextHandle EffectContext = MakeEffectContext(Handle, ActorInfo);
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ChaseEffect, GetAbilityLevel());
    if (!SpecHandle.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    SpecHandle.Data->SetContext(EffectContext);
    SpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MentalPoint, TriggerEventData ? TriggerEventData->EventMagnitude : 0.f);
    ActivChaseEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGA_ChaseActor::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (nullptr != TargetASC)
    {
        if (ActivChaseEffectHandle.IsValid())
        {
            TargetASC->RemoveActiveGameplayEffect(ActivChaseEffectHandle);
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

