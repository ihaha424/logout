// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AimItem.generated.h"

class UAbilityTask_PlayMontageAndWait;
class USplineComponent;
class USplineMeshComponent;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_AimItem : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AimItem();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void UpdateParabola();
	void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UFUNCTION()
	void OnMontageInterrupted();
	// ≈∏¿Ã∏”
	FTimerHandle UpdateTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HoldingItemMontage;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayHoldingItemMontageTask = nullptr;

	UPROPERTY()
	USplineComponent* SplineComp = nullptr;
	UPROPERTY()
	USkeletalMeshComponent* OwnerMeshComp = nullptr;
	UPROPERTY()
	AActor* OwnerActor = nullptr;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> SplineStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UMaterialInterface> SplineMaterial;
};
