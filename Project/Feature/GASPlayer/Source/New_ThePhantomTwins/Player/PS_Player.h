// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "PS_Player.generated.h"

class UPlayerAttributeSet;

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

	UFUNCTION(BlueprintCallable)
	bool IsRecovery() {return bIsRecovery;}
	void SetRecovery(bool IsRecovery);
	UFUNCTION(BlueprintCallable)
	bool IsGroggy() {return bIsGroggy;}
	void SetGroggy(bool IsGroggy);

protected:

	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(Replicated)
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	UPROPERTY(Replicated, EditAnywhere, Category = "Skill")
	FGameplayTag PassiveSkillTag;

	UPROPERTY(Replicated, EditAnywhere, Category = "Skill")
	FGameplayTag ActiveSkillTag;

	UPROPERTY(Replicated, EditAnywhere, Category = "Recovery")
	bool bIsRecovery = false;

	UPROPERTY(Replicated, EditAnywhere, Category = "Recovery")
	bool bIsGroggy = false;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UInventoryComponent> InventoryComp;
};
