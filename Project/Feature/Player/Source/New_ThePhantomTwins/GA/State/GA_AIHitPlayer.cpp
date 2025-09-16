// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AIHitPlayer.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_AIHitPlayer::UGA_AIHitPlayer()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
}

void UGA_AIHitPlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	HitMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HitMontage"), HitMontage, 1.0f);
	HitMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);
	HitMontageTask->ReadyForActivation();
}

void UGA_AIHitPlayer::OnMontageComplete()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	//NULLCHECK_CODE_RETURN_LOG(ASC, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);, );
	//FGameplayTagContainer RemoveTags;
	//RemoveTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
	//ASC->RemoveActiveEffectsWithTags(RemoveTags); 
	ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
