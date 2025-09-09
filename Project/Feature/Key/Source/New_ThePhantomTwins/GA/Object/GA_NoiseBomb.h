// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_NoiseBomb.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_NoiseBomb : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGA_NoiseBomb();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 소음폭탄 액터 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<class ANoiseBomb> NoiseBombClass;

    // 투척 거리 (목표까지의 거리)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float ThrowDistance = 1000.0f;

    // 궤적 아크 값 (0.0 - 1.0, 값이 클수록 높게 던짐)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    float Arc = 0.5f;

private:
    void SpawnNoiseBomb();
    FVector CalculateTargetLocation(const FVector& StartLocation);
    FVector GetLeftHandSocketLocation() const;
    FRotator GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const;
};
