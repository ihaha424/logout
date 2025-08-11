// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_CheckRangeHit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRangeHitResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UAT_CheckRangeHit : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAT_CheckRangeHit();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAT_CheckRangeHit* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ATA_RangeHit> TargetActorClass);
	virtual void Activate() override;
	virtual void OnDestroy(bool bAbilityEnded) override;

	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FRangeHitResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<class ATA_RangeHit> TargetActorClass;

	UPROPERTY()
	TObjectPtr<class ATA_RangeHit> SpawnedTargetActor;
};