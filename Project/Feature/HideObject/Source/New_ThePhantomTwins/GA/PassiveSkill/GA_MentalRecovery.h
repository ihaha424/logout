// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_MentalRecovery.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_MentalRecovery : public UGameplayAbility
{
	GENERATED_BODY()
public:
    UGA_MentalRecovery();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
 
protected:
    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<UGameplayEffect> MentalHealEffect;

    FTimerHandle HealTimerHandle;
    void HealTick();
};
