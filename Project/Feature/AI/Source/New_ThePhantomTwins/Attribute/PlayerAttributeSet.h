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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHUDDelegate, const int32, value);
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnRep_HP(const FGameplayAttributeData& OldValue);
	void OnRep_MaxHP(const FGameplayAttributeData& OldValue);
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	void OnRep_MentalPoint(const FGameplayAttributeData& OldValue);
	void OnRep_MaxMentalPoint(const FGameplayAttributeData& OldValue);
	void OnRep_CoreEnergy(const FGameplayAttributeData& OldValue);
	void OnRep_MaxCoreEnergy(const FGameplayAttributeData& OldValue);
	void OnRep_Speed(const FGameplayAttributeData& OldValue);
	void OnRep_SpeedAdjustment(const FGameplayAttributeData& OldValue);
	void OnRep_FinalSpeed(const FGameplayAttributeData& OldValue);
	void OnRep_ExecuteSkill(const FGameplayAttributeData& OldValue);
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FAttributeDelegate OnPlayerLowHP;
	mutable FAttributeDelegate OnPlayerDowned;
	mutable FAttributeDelegate OnPlayerConfused1st;
	mutable FAttributeDelegate OnPlayerConfused2nd;
	mutable FAttributeDelegate OnPlayerConfused3rd;
	mutable FAttributeDelegate OnPlayerUseSkill;
	mutable FAttributeDelegate OnMentalPointNotMax;

	mutable FHUDDelegate OnChangedHP;
	mutable FHUDDelegate OnChangedMentalPoint;
	mutable FHUDDelegate OnChangedCoreEnergy;
	mutable FHUDDelegate OnChangedStamina;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HP;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MentalPoint;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMentalPoint;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CoreEnergy;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxCoreEnergy;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Speed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpeedAdjustment;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FinalSpeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ExecuteSkill;

	bool bPlayerLowHP = false;
	bool bPlayerDowned = false;
	bool bPlayerConfused1st = false;
	bool bPlayerConfused2nd = false;
	bool bPlayerConfused3rd = false;
	bool bPlayerUseSkill = false;
	bool bMentalPointNotMax = false;
};
