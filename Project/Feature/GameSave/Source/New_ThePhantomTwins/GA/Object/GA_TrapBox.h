// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_TrapBox.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_TrapBox : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_TrapBox();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UseTrapBoxMontage;

private:
	TObjectPtr<class APlayerCharacter> Character = nullptr;
	TObjectPtr<AActor> TargetActor = nullptr;
	
};
