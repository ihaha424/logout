// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AimItem.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_AimItem : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AimItem();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HoldingItemMontage;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayHoldingItemMontageTask = nullptr;
};
