// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ExecuteActiveSkill.h"

#include "AbilitySystemComponent.h"
#include "New_ThePhantomTwins/Player/PlayerCharacter.h"
#include "New_ThePhantomTwins/Tags/TPTGameplayTags.h"

UGA_ExecuteActiveSkill::UGA_ExecuteActiveSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ExecuteActiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	APlayerCharacter* AbilityUser = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!AbilityUser)
	{
		return;
	}
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ExecuteSkillEffect, 1.f);
	EffectSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_UseSkill, 1);
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}