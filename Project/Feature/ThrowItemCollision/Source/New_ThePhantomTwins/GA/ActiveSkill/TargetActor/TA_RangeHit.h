// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Radius.h"
#include "TA_RangeHit.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ATA_RangeHit : public AGameplayAbilityTargetActor_Radius
{
	GENERATED_BODY()
public:
	ATA_RangeHit();
	virtual void StartTargeting(UGameplayAbility* InAbility) override;
	virtual void ConfirmTargetingAndContinue() override;
	FGameplayAbilityTargetDataHandle MakeTargetData() const;

	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

	bool bShowDebug = false;
};
