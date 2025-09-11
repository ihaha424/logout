// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Objects/ItemData.h"
#include "GA_ThrowItem.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_ThrowItem : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_ThrowItem();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 소음폭탄 액터 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
    TSubclassOf<class AThrowNoiseBomb> NoiseBombClass;

    // EMP 액터 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
    TSubclassOf<class AThrowEMP> EMPClass;

    // 투척 거리 (목표까지의 거리)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Noise")
    float ThrowDistance = 100.0f;

    // 궤적 아크 값 (0.0 - 1.0, 값이 클수록 높게 던짐)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
    float Arc = 0.5f;

private:
    void SpawnThrowableItem(EItemType ItemType);
    FVector CalculateTargetLocation(const FVector& StartLocation);
    FVector GetRightHandSocketLocation() const;
    FRotator GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const;
};
