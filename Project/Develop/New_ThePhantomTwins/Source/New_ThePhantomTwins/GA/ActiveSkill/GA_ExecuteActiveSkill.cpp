// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ExecuteActiveSkill.h"

#include "AbilitySystemComponent.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_ExecuteActiveSkill::UGA_ExecuteActiveSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_ActiveSkill);
	SetAssetTags(DefaultTags);
}

void UGA_ExecuteActiveSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	TPT_LOG(GALog, Error, TEXT("Start"));
	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SkillNumber = TriggerEventData->EventMagnitude;
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	//스프린트 스킬
	if (SkillNumber == 1)
	{
		FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
		if (CoolDownSpecHandle.IsValid())
		{
			CoolDownSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, 1.0f);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
		}
		ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_SkillCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);
	}

	//오라보기 스킬
	if (SkillNumber == 2)
	{
		FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
		if (CoolDownSpecHandle.IsValid())
		{
			CoolDownSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, 1.0f);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
		}
		ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_SkillCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);
	}
}

void UGA_ExecuteActiveSkill::OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
	bHasCoolDownTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false
	if (!bHasCoolDownTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
