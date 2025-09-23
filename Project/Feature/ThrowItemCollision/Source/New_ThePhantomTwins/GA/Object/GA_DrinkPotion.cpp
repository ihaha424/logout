// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_DrinkPotion.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/TPTGameplayTags.h"

UGA_DrinkPotion::UGA_DrinkPotion()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_DrinkPotion);
	SetAssetTags(DefaultTags);
}

void UGA_DrinkPotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_PlayMontageAndWait* PlayDrinkMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("DrinkMontage"), DrinkMontage, 1.0f);
	PlayDrinkMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);

	PlayDrinkMontageTask->ReadyForActivation();
}

void UGA_DrinkPotion::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
