#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "Objects/ItemData.h"
#include "Objects/FThrowItemDT.h"
#include "GA_ThrowItem.generated.h"

// Forward declarations
class AThrowNoiseBomb;
class AThrowEMP;
class UProjectileMovementComponent;
class UDataTable;

/**
 * UGA_ThrowItem
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_ThrowItem : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ThrowItem();

	UFUNCTION()
	void OnMontageComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ThrowItemMontage;
protected:
	// GameplayAbility overrides
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Projectile classes (set in blueprint/defaults) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	TSubclassOf<AThrowNoiseBomb> NoiseBombClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	TSubclassOf<AThrowEMP> EMPClass;

	/** DataTable containing FThrowItemDT rows */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThrowItem")
    TObjectPtr<UDataTable> ThrowItemDT;


	/**
	 * Arc parameter for SuggestProjectileVelocity_CustomArc
	 *
	 * 발사 궤적의 곡률(높이)을 조절하는 값
	 *  - 0.0f → 직선에 가까운 궤적 (낮게 던짐, 직사 포격 느낌)
	 *  - 0.5f → 중간 높이 포물선 (기본값, 자연스럽고 현실적인 곡선)
	 *  - 1.0f → 높게 던지는 포물선 (활 모양처럼 크게 띄움)
	 *
	 * 같은 시작점(StartLocation)과 목표점(TargetLocation)이라도
	 * Arc 값에 따라 궤적 높이가 달라짐
	 */
	 // 발사 궤적의 곡률(높이)을 조절하는 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	float Arc = 0.5f;


private:
	/** Spawn the throwable corresponding to the item type */
	void SpawnThrowableItem(EItemType ItemType);

	/** Calculate target location based on ThrowItemData and start location */
	FVector CalculateTargetLocation(const FThrowItemDT* ThrowItemData, const FVector& StartLocation) const;

	/** Get right hand socket world location (uses avatar mesh) */
	FVector GetRightHandSocketLocation() const;

	/** Determine spawn rotation (camera/controller/actor priority) */
	FRotator GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const;

	/** Initialize ProjectileMovementComponent with provided velocity/rotation */
	void InitializeProjectileMovement(UProjectileMovementComponent* ProjectileMovementComponent, const FVector& LaunchVelocity, const FRotator& SpawnRotation) const;

	/** Apply settings from DT (gravity scale, etc.) to projectile movement */
	void ApplyThrowItemDataSettings(UProjectileMovementComponent* ProjectileComp, const FThrowItemDT* ThrowItemData) const;

	/** Get FThrowItemDT row for given item type (returns nullptr if not found) */
	FThrowItemDT* GetThrowItemData(EItemType ItemType) const;

	/** Cancel AimItem ability if it's currently active */
	UFUNCTION()
	void CancelAimItemAbility();

};
