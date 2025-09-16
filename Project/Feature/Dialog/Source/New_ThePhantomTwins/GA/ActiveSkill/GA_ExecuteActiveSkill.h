// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ExecuteActiveSkill.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_ExecuteActiveSkill : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_ExecuteActiveSkill();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	UFUNCTION()
	void OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount);
	UFUNCTION()
	void ApplyEffect();

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> ExecuteSprintSkillEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> ExecuteOutLineSkillEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayEffect> CoolDownEffect;

	bool bHasCoolDownTag = false;
	float SkillNumber = -1.0f;
	float SprintSkillValue = 1.0f;
	float OutLineSkillValue = 1.0f;
	TArray<float> SkillValue = {0, 0};

};
