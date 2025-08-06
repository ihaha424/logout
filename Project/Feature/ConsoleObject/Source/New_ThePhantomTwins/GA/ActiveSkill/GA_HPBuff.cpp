// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_HPBuff.h"

#include "Tags/TPTGameplayTags.h"

UGA_HPBuff::UGA_HPBuff()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_HPBuff);
}

void UGA_HPBuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(HPBuffEffect, 1.0f);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
    }

    FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
    if (CoolDownSpecHandle.IsValid())
    {
        CoolDownSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, CoolDownValue);
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
