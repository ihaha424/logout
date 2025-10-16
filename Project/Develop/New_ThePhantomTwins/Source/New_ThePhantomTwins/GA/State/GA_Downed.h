// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Downed.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_Downed : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Downed();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void OnDownedTagChanged(const FGameplayTag Tag, int32 TagCount);
	void SetSpeed(float Speed, const FGameplayAbilityActorInfo* ActorInfo);
protected:
	const FGameplayAbilityActorInfo* GAActorInfo;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> SetSpeedEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	float DownedSpeed = 30.f;

	FVector DefaultSocketOffset;
};
