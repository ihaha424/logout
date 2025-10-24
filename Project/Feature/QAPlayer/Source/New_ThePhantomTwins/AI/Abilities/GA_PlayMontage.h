// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PlayMontage.generated.h"

class UAnimMontage;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_PlayMontage : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_PlayMontage();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted,
        FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        FGameplayAbilityActivationInfo ActivationInfo);

    UFUNCTION()
    void OnCompleteCallback();
    UFUNCTION()
    void OnInterruptedCallback();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> AnimationMontage;

    static bool StopCurrentOrJumpIfSame(
        const FGameplayAbilityActorInfo* ActorInfo,
        UAnimMontage* Montage,
        const FName SectionToJump,
        float StopBlendOut = 0.1f);
};
