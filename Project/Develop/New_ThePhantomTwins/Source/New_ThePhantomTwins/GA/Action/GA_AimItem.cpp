// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AimItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_AimItem::UGA_AimItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem);
}

void UGA_AimItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TPT_LOG(GALog, Error, TEXT(""));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayHoldingItemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HoldingItemMontage"), HoldingItemMontage, 1.0f);
	PlayHoldingItemMontageTask->ReadyForActivation();
}
