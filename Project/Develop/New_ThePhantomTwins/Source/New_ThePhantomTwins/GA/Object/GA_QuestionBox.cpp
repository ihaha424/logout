// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_QuestionBox.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_QuestionBox::UGA_QuestionBox()
{

}

void UGA_QuestionBox::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
