// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/State/GA_Confused2nd.h"
#include "Log/TPTLog.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"

UGA_Confused2nd::UGA_Confused2nd()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
}

void UGA_Confused2nd::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, );
	// 점차 정신력이 좋아질때를 대비해서 태그 떼기.
	ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
	// 점차 정신력이 안좋아질때를 대비해서 태그 떼기.
	ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);

	ASC->AddGameplayCue(SoundCueTag, FGameplayCueParameters());
}

void UGA_Confused2nd::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	TPT_LOG(GALog, Error, TEXT(""));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, );
	ASC->RemoveGameplayCue(SoundCueTag);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
