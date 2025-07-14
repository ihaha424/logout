// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"

UPlayerAttributeSet::UPlayerAttributeSet() :
	HP(100),
	MaxHP(100),
	MentalPoint(100),
	MaxMentalPoint(100),
	CoreEnergy(5),
	MaxCoreEnergy(5),
	Stamina(100),
	MaxStamina(100),
	Speed(120)
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
	float MinimumHealth = 0.0f;

	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(), MinimumHealth, GetMaxHP()));
	}
	if (Data.EvaluatedData.Attribute == GetMentalPointAttribute())
	{
		SetMentalPoint(FMath::Clamp(GetMentalPoint(), MinimumHealth, GetMaxMentalPoint()));
	}
	if (GetHP() <= 0.0f && !bPlayerDowned)
	{// 적용하고 잇는 대상에다가 ASC로 명확하게 매뉴얼로 태그를 지정해주고 잇따.
		//Data.Target.AddLooseGameplayTag(ABTAG_CHARACTER_ISDEAD); // 이게 부착이 될것이다.
		OnPlayerDowned.Broadcast();
	}
	bPlayerDowned = GetHP() <= 0.0f;
}
