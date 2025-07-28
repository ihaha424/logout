// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "New_ThePhantomTwins/Player/PS_Player.h"

UPlayerAttributeSet::UPlayerAttributeSet() :
	MaxHP(100),
	MaxMentalPoint(100),
	MaxCoreEnergy(5),
	MaxStamina(100),
	Speed(120),
	SpeedAdjustment(0),
	FinalSpeed(0),
	ExecuteSkill(-1)
{
	InitHP(GetMaxHP());
	InitMentalPoint(GetMaxMentalPoint());
	InitCoreEnergy(GetMaxCoreEnergy());
	InitStamina(GetMaxStamina());
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetMaxHPAttribute())
	{
		SetHP(NewValue);
	}
	if (Attribute == GetMaxMentalPointAttribute())
	{
		SetMentalPoint(NewValue);
	}
	if (Attribute == GetMaxStaminaAttribute())
	{
		SetStamina(NewValue);
	}
	if (Attribute == GetMaxCoreEnergyAttribute())
	{
		SetCoreEnergy(NewValue);
	}
}

bool UPlayerAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{	
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{	// 현재 착란 3단계면 체력회복이 불가하도록 막음.
			if (Data.Target.HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd))
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false; // 이 이펙트의 실행을 전부 중지시키기 위해서 false 반환.
			}
		}
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
		OnChangedHP.Broadcast(GetHP());
	}

	if (Data.EvaluatedData.Attribute == GetMentalPointAttribute())
	{
		SetMentalPoint(FMath::Clamp(GetMentalPoint(), MinimumPoint, GetMaxMentalPoint()));
		OnChangedMentalPoint.Broadcast(GetMentalPoint());
	}

	if (Data.EvaluatedData.Attribute == GetCoreEnergyAttribute())
	{
		SetCoreEnergy(FMath::Clamp(GetCoreEnergy(), MinimumPoint, GetMaxCoreEnergy()));
		OnChangedCoreEnergy.Broadcast(GetCoreEnergy());
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), MinimumPoint, GetMaxStamina()));
		OnChangedStamina.Broadcast(GetStamina());
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

	// 체력이 MaxHp의 30%이하라면 Low HP 효과 발동.
	if (GetHP() < GetMaxHP() * 0.3f && !bPlayerLowHP && GetHP() > 0.0)
	{
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
		OnPlayerLowHP.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
	}
	bPlayerLowHP = GetHP() < GetMaxHP() * 0.3f && GetHP() > 0.0;
	// 체력이 0이하라면 다운.
	if (GetHP() <= 0.0f && !bPlayerDowned)
	{
		Data.Target.AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		OnPlayerDowned.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		TPT_LOG(PlayerLog,Error,TEXT(" %s "), *GetNameSafe(Data.Target.GetOwner()));
	}
	bPlayerDowned = GetHP() <= 0.0f;

	// 정신력이 MAX가 아니라면 거리별회복 GA 호출
	if (GetMentalPoint() < GetMaxMentalPoint() && !bMentalPointNotMax)
	{
		Data.Target.AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
		OnMentalPointNotMax.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
	}
	bMentalPointNotMax = GetMentalPoint() < GetMaxMentalPoint();
	// 정신력 이상 단계가 아니게 되면 태그 떼주기.
	if (GetMentalPoint() > 50.0f)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
	}
	// 정신력이 50 이하라면 착란 1단계
	if (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f && !bPlayerConfused1st)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
		OnPlayerConfused1st.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
	}
	bPlayerConfused1st = (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f);

	// 정신력이 25 이하라면 착란 2단계
	if (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f && !bPlayerConfused2nd)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		OnPlayerConfused2nd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
	}
	bPlayerConfused2nd = (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f);

	// 정신력이 0 이라면 착란 3단계
	if (GetMentalPoint() <= 0.0f && !bPlayerConfused3rd)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
		OnPlayerConfused3rd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
	}
	bPlayerConfused3rd = GetMentalPoint() <= 0.0f;

	// 스킬발동이 true가 되면 스킬실행.
	if (GetExecuteSkill() > 0 && !bPlayerUseSkill)
	{
		OnPlayerUseSkill.Broadcast(Cast<APS_Player>(GetOwningActor())->GetActiveSkillTag());
	}
	bPlayerUseSkill = GetExecuteSkill() > 0;
}
