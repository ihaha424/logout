// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "PS_Player.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APS_Player : public APlayerState, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	APS_Player();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

protected:

	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(Replicated)
	TObjectPtr<class UPlayerAttributeSet> AttributeSet;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill")
	FGameplayTag PassiveSkill;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill")
	FGameplayTag ActiveSkill;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray< /* ¾ÆÀÌÅÛ Enum */ int32> ItemSlots;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
