// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../Tags/TPTGameplayTags.h"

UPlayerAttributeSet::UPlayerAttributeSet() :
	HP(100),
	MaxHP(100),
	MentalPoint(100),
	MaxMentalPoint(100),
	CoreEnergy(5),
	MaxCoreEnergy(5),
	Stamina(100),
	MaxStamina(100),
	Speed(120),
	SpeedAdjustment(0),
	FinalSpeed(0)
{
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{

}

bool UPlayerAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	return true;
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	float MinimumPoint = 0.0f;

	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(), MinimumPoint, GetMaxHP()));
	}

	if (Data.EvaluatedData.Attribute == GetMentalPointAttribute())
	{
		SetMentalPoint(FMath::Clamp(GetMentalPoint(), MinimumPoint, GetMaxMentalPoint()));
	}

	if (Data.EvaluatedData.Attribute == GetCoreEnergyAttribute())
	{
		SetCoreEnergy(FMath::Clamp(GetCoreEnergy(), MinimumPoint, GetMaxCoreEnergy()));
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), MinimumPoint, GetMaxStamina()));
	}
	// 스피드나 증감스피드가 변경될때 마다  final 스피드가 업데이트 되도록함.
	if (Data.EvaluatedData.Attribute == GetSpeedAttribute())
	{
		SetSpeed(FMath::Clamp(GetSpeed(), MinimumPoint, 10000));
		SetFinalSpeed(FMath::Clamp(GetSpeed() + GetSpeedAdjustment(), MinimumPoint, 10000));
	}

	if (Data.EvaluatedData.Attribute == GetSpeedAdjustmentAttribute())
	{
		SetFinalSpeed(FMath::Clamp(GetSpeed() + GetSpeedAdjustment(), MinimumPoint, 10000));
	}

	// 체력이 0이하라면 다운.
	if (GetHP() <= 0.0f && !bPlayerDowned)
	{
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		OnPlayerDowned.Broadcast();
	}
	bPlayerDowned = GetHP() <= 0.0f;

	// 정신력이 0이하라면 착란.
	if (GetMentalPoint() <= 0.0f && !bPlayerConfused)
	{
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused);
		OnPlayerConfused.Broadcast();
	}
	bPlayerConfused = GetMentalPoint() <= 0.0f;
}
