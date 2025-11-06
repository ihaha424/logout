// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ScanCloestPlayer.generated.h"

class UNavigationQueryFilter;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_ScanCloestPlayer : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ScanCloestPlayer();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    bool IsReachable(UWorld* World, APawn* Pawn, const FVector& Goal, TSubclassOf<UNavigationQueryFilter> Filter = nullptr);
};
