// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AIHitPlayer.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_AIHitPlayer : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AIHitPlayer();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	void OffSound(const FGameplayTag InputTag, int32 Count);
	UFUNCTION()
	void OnMontageComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitMontage;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* HitMontageTask = nullptr;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue;

	UPROPERTY()
	UAudioComponent* ActiveAudioComponent = nullptr;
};
