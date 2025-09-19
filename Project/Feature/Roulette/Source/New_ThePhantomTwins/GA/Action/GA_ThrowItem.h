#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Objects/ItemData.h"
#include "Objects/FThrowItemDT.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThrowItem")
    TObjectPtr<UDataTable> ThrowItemDT;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowItem")
	float Arc = 0.5f;

private:
	void SpawnThrowableItem(EItemType ItemType);
	FVector CalculateTargetLocation(const FThrowItemDT* ThrowItemData, const FVector& StartLocation) const;
	FVector GetRightHandSocketLocation() const;
	FRotator GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const;
	void InitializeProjectileMovement(class UProjectileMovementComponent* ProjectileMovementComponent, const FVector& LaunchVelocity, const FRotator& SpawnRotation) const;
	void ApplyThrowItemDataSettings(UProjectileMovementComponent* ProjectileComp, const FThrowItemDT* ThrowItemData) const;
	
	// DataTable에서 ItemType과 맞는 row를 가져오는 함수
	FThrowItemDT* GetThrowItemData(EItemType ItemType) const;
};
