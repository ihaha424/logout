// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LowHP.generated.h"

class UPostProcessComponent;


UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_LowHP : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_LowHP();

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	const FGameplayAbilityActorInfo* GAActorInfo;
	TObjectPtr<UPostProcessComponent> PPComp;

	void OnLowHPTagChanged(const FGameplayTag Tag, int32 TagCount);

	void UpdateFade();
	FTimerHandle FadeTimerHandle;
	float CurrentWeight;
	float FinalWeight;
	float InterpSpeed = 2.0f;
};
