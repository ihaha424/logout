// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Sprint.h"

#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_Sprint::UGA_Sprint()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_Sprint);
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    TPT_LOG(GALog, Error, TEXT("5"));
    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(SprintEffect, 1.0f);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
    }
}
