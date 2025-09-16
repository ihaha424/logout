// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_HealBandage.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/TPTGameplayTags.h"

UGA_HealBandage::UGA_HealBandage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_HealBandage);
}

void UGA_HealBandage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_PlayMontageAndWait* PlayBandageMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BandageMontage"), BandageMontage, 1.0f);
	PlayBandageMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);

	PlayBandageMontageTask->ReadyForActivation();
}

void UGA_HealBandage::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
