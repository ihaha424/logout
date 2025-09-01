// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h" 
#include "Objects/ItemData.h"
#include "GA_HoldItem.generated.h"
	
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_HoldItem : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_HoldItem();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UDataTable> ItemAbilityTable;

private:
	TObjectPtr<class UStaticMesh> SetItemStaticMesh(EItemType ItemType);

};
