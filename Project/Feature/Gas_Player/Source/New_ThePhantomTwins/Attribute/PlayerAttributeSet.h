// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeDelegate, const FGameplayTag, InputTag);

UCLASS()
class NEW_THEPHANTOMTWINS_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPlayerAttributeSet();

	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, HP);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHP);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MentalPoint);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxMentalPoint);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CoreEnergy);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxCoreEnergy);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Speed);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SpeedAdjustment);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FinalSpeed);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ExecuteSkill);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FAttributeDelegate OnPlayerDowned;
	mutable FAttributeDelegate OnPlayerConfused;
	mutable FAttributeDelegate OnPlayerUseSkill;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MentalPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMentalPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CoreEnergy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxCoreEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpeedAdjustment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FinalSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ExecuteSkill;

	bool bPlayerDowned = false;
	bool bPlayerConfused = false;
	bool bPlayerUseSkill = false;
};
