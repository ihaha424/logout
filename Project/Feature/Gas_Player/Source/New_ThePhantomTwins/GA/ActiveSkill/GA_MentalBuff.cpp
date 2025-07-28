// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MentalBuff.h"

UGA_MentalBuff::UGA_MentalBuff()
{
}

void UGA_MentalBuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
