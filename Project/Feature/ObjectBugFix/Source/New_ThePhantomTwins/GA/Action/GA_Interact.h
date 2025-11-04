// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Interact.generated.h"

class APlayerCharacter;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Interact();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled) override;
	// NetWork
	UFUNCTION(Server, Reliable)
	void C2S_Interact(UObject* interact, AActor* Owner);
	void C2S_Interact_Implementation(UObject* interact, AActor* Owner);

	UFUNCTION(BlueprintCallable)
	void OnMontageComplete();
	void InteractExecute();
	void ClearAllTimers();

	UPROPERTY()
	APlayerCharacter* Character = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	// 府目滚府 包访 函荐
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	FTimerHandle CompleteHandle;
	FTimerHandle UpdateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> InteractMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> RecoveryMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue;

	UPROPERTY()
	UAudioComponent* ActiveAudioComponent = nullptr;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayInteractMontageTask = nullptr;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayRecoveryMontageTask = nullptr;
	UPROPERTY()
	UAnimMontage* CurrentPlayingMontage = nullptr;

	UPROPERTY(EditAnywhere)
	float Distance = 100.f;

	bool bEnding = false;
};
