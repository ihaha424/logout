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
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill);
}

void UGA_ExecuteActiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	SkillValue = 1.0f;
	ApplyEffect();

	FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);

	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_SkillCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);
}

void UGA_ExecuteActiveSkill::OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
	bHasCoolDownTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false
	if (!bHasCoolDownTag)
	{
		SkillValue = -1.0f;
		ApplyEffect();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_ExecuteActiveSkill::ApplyEffect()
{
	FGameplayEffectSpecHandle ExecuteSkillSpecHandle = MakeOutgoingGameplayEffectSpec(ExecuteSkillEffect, 1.f);
	ExecuteSkillSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_UseSkill, SkillValue);

	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, ExecuteSkillSpecHandle);
}