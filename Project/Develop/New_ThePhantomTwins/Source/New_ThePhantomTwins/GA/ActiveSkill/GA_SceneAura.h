// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SceneAura.generated.h"


class APlayerCharacter;

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_SceneAura : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGA_SceneAura();

    virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                        FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:
   
    void ScanTargets();

    // Aura 
    void ApplyAuraToTarget(AActor* Target);
    void RemoveAuraFromTarget(AActor* Target);
	bool IsValidAuraTarget(AActor* Target) const;
    bool IsCameraBlocked();
    void SpawnScanEffectActor();

    UFUNCTION()
    void OnSceneAuraTagChanged(const FGameplayTag InputTag, int32 TagCount);
    UFUNCTION()
    void OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount);

    // 블루프린트에서 지정할 수 있도록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan Effect")
    TSubclassOf<AActor> ScanEffectActorClass;

    TArray<AActor*> UnlimitedObjects;

    // 타이머
    FTimerHandle ScanTimerHandle;

    // 현재 Aura가 적용된 대상들
    TSet<TWeakObjectPtr<AActor>> CurrentAuraTargets;

	AActor* OwnerActor = nullptr;

    // === Configurable Variables ===

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<UGameplayEffect> SceneAuraEffect;

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<UGameplayEffect> CoolDownEffect;

    UPROPERTY(EditDefaultsOnly, Category="Aura")
    float SenseRadius = 5000.f; 

    UPROPERTY(EditDefaultsOnly, Category="Aura")
    float ScanInterval = 0.5f; 

    bool bHasCoolDownTag = false;
private:
    FCollisionShape Sphere;
};
