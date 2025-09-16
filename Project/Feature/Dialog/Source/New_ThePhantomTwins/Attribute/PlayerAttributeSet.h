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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttValueDelegate, const int32, value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttTagDelegate, const FGameplayTag, InputTag);

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
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ExecuteSprintSkill);
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ExecuteOutLineSkill);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void OnRep_HP(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHP(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MentalPoint(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMentalPoint(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_CoreEnergy(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxCoreEnergy(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_SpeedAdjustment(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FinalSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ExecuteSprintSkill(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_ExecuteOutLineSkill(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FAttTagDelegate OnPlayerLowHP;
	mutable FAttTagDelegate OnPlayerDowned;
	mutable FAttTagDelegate OnPlayerConfused1st;
	mutable FAttTagDelegate OnPlayerConfused2nd;
	mutable FAttTagDelegate OnPlayerConfused3rd;
	mutable FAttTagDelegate OnPlayerUseSkill;
	mutable FAttTagDelegate OnMentalPointNotMax;

	mutable FAttValueDelegate OnChangedHP;
	mutable FAttValueDelegate OnChangedMentalPoint;
	mutable FAttValueDelegate OnChangedCoreEnergy;
	mutable FAttValueDelegate OnChangedStamina;
	mutable FAttValueDelegate OnFullStamina;
	mutable FAttValueDelegate OnChangedSpeed;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_HP, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HP;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHP, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHP;

	UPROPERTY(ReplicatedUsing = OnRep_MentalPoint, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MentalPoint;
	UPROPERTY(ReplicatedUsing = OnRep_MaxMentalPoint, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMentalPoint;

	UPROPERTY(ReplicatedUsing = OnRep_CoreEnergy, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CoreEnergy;
	UPROPERTY(ReplicatedUsing = OnRep_MaxCoreEnergy, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxCoreEnergy;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(ReplicatedUsing = OnRep_MaxStamina, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(ReplicatedUsing = OnRep_Speed, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Speed;

	UPROPERTY(ReplicatedUsing = OnRep_SpeedAdjustment, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpeedAdjustment;

	UPROPERTY(ReplicatedUsing = OnRep_FinalSpeed, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FinalSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_ExecuteSprintSkill, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ExecuteSprintSkill;

	UPROPERTY(ReplicatedUsing = OnRep_ExecuteOutLineSkill, BlueprintReadOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ExecuteOutLineSkill;

	bool bPlayerLowHP = false;
	bool bPlayerDowned = false;
	bool bPlayerConfused1st = false;
	bool bPlayerConfused2nd = false;
	bool bPlayerConfused3rd = false;
	bool bPlayerUseSprintSkill = false;
	bool bPlayerUseOutLineSkill = false;
	bool bMentalPointNotMax = false;
	bool bFullStamina = false;
};
