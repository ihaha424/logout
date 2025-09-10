// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SceneAura.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_SceneAura : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGA_SceneAura();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
   
    void ScanTargets();

    // Aura 
    void ApplyAuraToTarget(AActor* Target);
    void RemoveAuraFromTarget(AActor* Target);

    // 타이머
    FTimerHandle ScanTimerHandle;
    FTimerHandle DurationTimerHandle;

    // 현재 Aura가 적용된 대상들
    TSet<TWeakObjectPtr<AActor>> CurrentAuraTargets;

    // === Configurable Variables ===
    UPROPERTY(EditDefaultsOnly, Category="Aura")
    float AuraDuration = 20.f;

    UPROPERTY(EditDefaultsOnly, Category="Aura")
    float SenseRadius = 5000.f; 

    UPROPERTY(EditDefaultsOnly, Category="Aura")
    float ScanInterval = 0.5f; 

private:
    FCollisionShape Sphere;
};
