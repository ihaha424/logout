// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ExecuteActiveSkill.h"

#include "AbilitySystemComponent.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"

UGA_ExecuteActiveSkill::UGA_ExecuteActiveSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ExecuteActiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	APlayerCharacter* AbilityUser = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(AbilityUser, GALog, Error, );
	// 코어에너지가 0이면 스킬사용 불가.
	if (ASC->GetSet<UPlayerAttributeSet>()->GetCoreEnergy() <= 0)
	{
		TPT_LOG(GALog, Error, TEXT("GetCoreEnergy() <= 0"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ExecuteSkillEffect, 1.f);

	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	TPT_LOG(GALog, Error, TEXT(""));
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}