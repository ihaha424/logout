// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/SpringArmComponent.h"
#include "GA_LookBack.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_LookBack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION()
	void TickMoveSpringArm(USpringArmComponent* SpringArm);
	void StopMoveAndEnd();

private:
	FVector OriginalLocation;
	bool bIsMoveLeft = false;

	UPROPERTY(EditAnywhere, Category = "MoveLeft")
	float MoveDistance = 50.f;
	float PositionY;

	FTimerHandle MoveTimerHandle;

	UPROPERTY(EditAnywhere, Category = "MoveLeft")
	float MoveTotalTime = 0.3f;
	float MoveElapsed = 0.f;
	FVector MoveStart;
	FVector MoveTarget;
	bool bMoving = false;
};
