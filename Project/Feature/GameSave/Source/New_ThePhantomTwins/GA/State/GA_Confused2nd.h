// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Confused2nd.generated.h"

/**
 *
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_Confused2nd : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Confused2nd();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	void OffSound(FGameplayTag InputTag, int32 Count);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue1st;
	UPROPERTY()
	UAudioComponent* ActiveAudioComponent1st = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue2nd;
	UPROPERTY()
	UAudioComponent* ActiveAudioComponent2nd = nullptr;
};
