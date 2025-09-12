#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Objects/ItemData.h"
#include "GA_ThrowItem.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_ThrowItem : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ThrowItem();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	TSubclassOf<class AThrowNoiseBomb> NoiseBombClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	TSubclassOf<class AThrowEMP> EMPClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ThrowItem")
	float ThrowDistance = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	float Arc = 0.5f;

private:
	void SpawnThrowableItem(EItemType ItemType);
	FVector CalculateTargetLocation(const FVector& StartLocation) const;
	FVector GetRightHandSocketLocation() const;
	FRotator GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const;

	void InitializeProjectileMovement(class UProjectileMovementComponent* ProjectileMovementComponent, const FVector& LaunchVelocity, const FRotator& SpawnRotation) const;
};