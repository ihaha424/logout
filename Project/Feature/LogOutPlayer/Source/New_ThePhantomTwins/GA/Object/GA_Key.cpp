// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Object/GA_Key.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"


UGA_Key::UGA_Key()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Object_Key);
}

void UGA_Key::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TPT_LOG(GALog, Log, TEXT("UGA_Key :: ActivateAbility()"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_PlayMontageAndWait* PlayDrinkMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UseKeyMontage"), UseKeyMontage, 1.0f);
	PlayDrinkMontageTask->OnCompleted.AddDynamic(this, &UGA_Key::OnMontageComplete);

	PlayDrinkMontageTask->ReadyForActivation();
}

void UGA_Key::OnMontageComplete()
{
	TPT_LOG(GALog, Error, TEXT(""));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
