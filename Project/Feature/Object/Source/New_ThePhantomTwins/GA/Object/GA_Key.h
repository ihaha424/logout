#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Key.generated.h"


UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_Key : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Key();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UseKeyMontage;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseKey = false;
private:
	TObjectPtr<class APlayerCharacter> Character = nullptr;
	TObjectPtr<AActor> TargetActor = nullptr;
};
