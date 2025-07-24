// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "PS_Player.generated.h"

class UPlayerAttributeSet;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APS_Player : public APlayerState, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	APS_Player();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	FGameplayTag GetActiveSkillTag() const { return ActiveSkillTag;}
	FGameplayTag GetPassiveSkillTag() const { return PassiveSkillTag;}
	
public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<class UInventoryComponent> InventoryComp;

protected:
	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(Replicated)
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	UPROPERTY(Replicated, EditAnywhere, Category = "Skill")
	FGameplayTag PassiveSkillTag;

	UPROPERTY(Replicated, EditAnywhere, Category = "Skill")
	FGameplayTag ActiveSkillTag;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
	TArray< /* ¾ÆÀÌÅÛ Enum */ int32> ItemSlots;


private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
