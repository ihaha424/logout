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
    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
   
    void ScanTargets();

    // Aura 
    void ApplyAuraToTarget(AActor* Target, int32 Value);
    void RemoveAuraFromTarget(AActor* Target);
	bool IsValidAuraTarget(AActor* Target) const;
    bool IsCameraBlocked();
    void SpawnScanEffectActor();

    UFUNCTION()
    void OnSceneAuraTagChanged(const FGameplayTag InputTag, int32 TagCount);
    UFUNCTION()
    void OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount);

    UFUNCTION(BlueprintImplementableEvent)
	void CallCoreEnergyZeroDialog(const APlayerCharacter* Player) const;

    // 블루프린트에서 지정할 수 있도록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scan Effect")
    TSubclassOf<AActor> ScanEffectActorClass;

    UPROPERTY()
    TArray<AActor*> UnlimitedObjects;

    AActor* OtherPlayer = nullptr;

    // 타이머
    FTimerHandle ScanTimerHandle;

    // 현재 Aura가 적용된 대상들
    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> CurrentAuraTargets;

	AActor* OwnerActor = nullptr;

    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> NewTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	USoundBase* SoundCue;

	UPROPERTY()
	UAudioComponent* ActiveAudioComponent = nullptr;

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
