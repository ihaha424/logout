// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Run.generated.h"


class UGameplayEffect;
 
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_Run : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Run();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	void StaminaDrain();
	void StaminaRegen();
	void SetSpeed(float Speed, const FGameplayAbilityActorInfo* ActorInfo);
	void OnSprintTagChanged(const FGameplayTag Tag, int32 TagCount);
	void OnConfusedTagChanged(const FGameplayTag Tag, int32 TagCount);
	void OffSound(const FGameplayTag Tag, int32 TagCount);

protected:

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> StaminaDrainEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> StaminaRegenEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> SetSpeedEffect;

	FTimerHandle StaminaCheckHandle;
	FTimerHandle MovementCheckHandle;

	FGameplayTagContainer CancelTags;
	UPROPERTY(EditAnywhere, Category = "GAS")
	float BaseRunSpeed = 450.0f;

	float OutPutRunSpeed = 450.0f;
	UPROPERTY(EditAnywhere, Category = "GAS")
	float SprintMultiplier = 1.5;
	bool bHasSprintTag = false;

	const FGameplayAbilityActorInfo* GAActorInfo;

	UPROPERTY()
	UAudioComponent* DrainAudioComponent = nullptr;
	UPROPERTY()
	UAudioComponent* ZeroAudioComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound");
	USoundBase* StaminaDrainSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound");
	USoundBase* StaminaZeroSound;

	bool bIsStaminaRegen = false;
};
