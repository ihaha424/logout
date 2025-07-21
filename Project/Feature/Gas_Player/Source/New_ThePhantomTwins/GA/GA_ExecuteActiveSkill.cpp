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
	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	APlayerCharacter* AbilityUser = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(AbilityUser, GALog, Error, );
	// 코어에너지가 0이면 스킬사용 불가.
	if (ASC->GetSet<UPlayerAttributeSet>()->GetCoreEnergy() <= 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	CoreEnergy = UseCoreEnergy;
	SkillValue = 1.0f;
	ApplyEffect();
	FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);

	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_SkillCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);
}

void UGA_ExecuteActiveSkill::OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
	TPT_LOG(GALog, Error, TEXT("%d"), TagCount);
	bHasCoolDownTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false
	if (!bHasCoolDownTag)
	{
		TPT_LOG(GALog, Error, TEXT("EndAbility  %d"), TagCount);
		CoreEnergy = NotUseCoreEnergy;
		SkillValue = -1.0f;
		ApplyEffect();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_ExecuteActiveSkill::ApplyEffect()
{
	FGameplayEffectSpecHandle ExecuteSkillSpecHandle = MakeOutgoingGameplayEffectSpec(ExecuteSkillEffect, 1.f);
	ExecuteSkillSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_UseSkill, SkillValue);
	ExecuteSkillSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoreEnergy, CoreEnergy);

	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, ExecuteSkillSpecHandle);
}