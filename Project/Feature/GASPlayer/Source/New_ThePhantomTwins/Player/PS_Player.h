// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PS_Player.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APS_Player : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	APS_Player();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UPlayerAttributeSet> AttributeSet;

};
