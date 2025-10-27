// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SmashObstacle.generated.h"

class AAIBaseCharacter;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_SmashObstacle : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_SmashObstacle();

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
    TObjectPtr<UAnimMontage> AttackMontage;
private:
    bool bActiveAbility = false;
    AActor* Target = nullptr;
    AAIBaseCharacter* OwnerPawn = nullptr;
    int32 CurCount = 0;
    int32 Count = 0;
};
