// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_StarterKit.generated.h"




UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_StarterKit : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_StarterKit();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
