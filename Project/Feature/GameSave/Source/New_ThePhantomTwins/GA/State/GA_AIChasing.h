// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AIChasing.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_AIChasing : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AIChasing();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	void OffSound(const FGameplayTag InputTag, int32 Count);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue;

	UPROPERTY()
	UAudioComponent* ActiveAudioComponent = nullptr;
};
