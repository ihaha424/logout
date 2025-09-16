// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "AIBaseAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAIAnimationActionState : uint8
{
	None,
	Attack,
	OpenObstacle,
	SmashObstacle
};


class UAbilitySystemComponent;

UCLASS()
class NEW_THEPHANTOMTWINS_API UAIBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

};
