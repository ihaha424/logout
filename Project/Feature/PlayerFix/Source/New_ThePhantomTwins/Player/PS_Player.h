// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Data/CharacterType.h"
#include "PS_Player.generated.h"

struct FItemSlot;
class UPlayerAttributeSet;

UCLASS()
class NEW_THEPHANTOMTWINS_API APS_Player : public APlayerState, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	APS_Player();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	//virtual void CopyProperties(APlayerState* PlayerState)override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	void SetIdentifyCharacterData();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CharacterType")
	FIdentifyCharacterData IdentifyCharacterData;

	UPROPERTY(Replicated, EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(Replicated)
	TObjectPtr<UPlayerAttributeSet> AttributeSet;
	// 인벤토리 스냅샷(값): 네트워크로 유지/이관될 데이터
	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	TArray<FItemSlot> InventorySnapshot;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UInventoryComponent> InventoryComp;
};
