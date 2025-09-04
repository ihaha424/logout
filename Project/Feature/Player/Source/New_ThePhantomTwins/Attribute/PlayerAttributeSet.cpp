// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "../Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Net/UnrealNetwork.h"
#include "Player/PS_Player.h"

UPlayerAttributeSet::UPlayerAttributeSet() :
	MaxHP(100),
	MaxMentalPoint(100),
	MaxCoreEnergy(5),
	MaxStamina(100),
	Speed(120),
	SpeedAdjustment(0),
	FinalSpeed(0),
	ExecuteSprintSkill(-1),
	ExecuteOutLineSkill(-1)
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
void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, HP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MentalPoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxMentalPoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, CoreEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxCoreEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SpeedAdjustment, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, FinalSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ExecuteSprintSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ExecuteOutLineSkill, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::OnRep_HP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, HP, OldValue);
	OnChangedHP.Broadcast(GetHP());

	// 체력이 MaxHp의 30%이하라면 Low HP 효과 발동.
	if (GetHP() < GetMaxHP() * 0.3f && !bPlayerLowHP)
	{
		OnPlayerLowHP.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
	}
	// 체력이 0이하라면 다운.
	if (GetHP() <= 0.0f && !bPlayerDowned)
	{
		OnPlayerDowned.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
	}
	bPlayerDowned = GetHP() <= 0.0f;
}
void UPlayerAttributeSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHP, OldValue); }
void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldValue);
	OnChangedStamina.Broadcast(GetStamina());
}
void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, OldValue); }
void UPlayerAttributeSet::OnRep_MentalPoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MentalPoint, OldValue);
	OnChangedMentalPoint.Broadcast(GetMentalPoint());

	// 정신력이 MAX가 아니라면 거리별회복 GA 호출
	if (GetMentalPoint() < GetMaxMentalPoint() && !bMentalPointNotMax)
	{
		OnMentalPointNotMax.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
	}
	bMentalPointNotMax = GetMentalPoint() < GetMaxMentalPoint();

	// 정신력이 50 이하라면 착란 1단계
	if (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f && !bPlayerConfused1st)
	{
		OnPlayerConfused1st.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
	}
	bPlayerConfused1st = (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f);

	// 정신력이 25 이하라면 착란 2단계
	if (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f && !bPlayerConfused2nd)
	{
		OnPlayerConfused2nd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
	}
	bPlayerConfused2nd = (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f);

	// 정신력이 0 이라면 착란 3단계
	if (GetMentalPoint() <= 0.0f && !bPlayerConfused3rd)
	{
		OnPlayerConfused3rd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
	}
	bPlayerConfused3rd = GetMentalPoint() <= 0.0f;
}
void UPlayerAttributeSet::OnRep_MaxMentalPoint(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxMentalPoint, OldValue); }
void UPlayerAttributeSet::OnRep_CoreEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, CoreEnergy, OldValue);
	OnChangedCoreEnergy.Broadcast(GetCoreEnergy());
}
void UPlayerAttributeSet::OnRep_MaxCoreEnergy(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxCoreEnergy, OldValue); }
void UPlayerAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Speed, OldValue);
	OnChangedSpeed.Broadcast(GetSpeed());
}
void UPlayerAttributeSet::OnRep_SpeedAdjustment(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SpeedAdjustment, OldValue); }
void UPlayerAttributeSet::OnRep_FinalSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, FinalSpeed, OldValue);
	//OnChangedSpeed.Broadcast(GetFinalSpeed());
}
void UPlayerAttributeSet::OnRep_ExecuteSprintSkill(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ExecuteSprintSkill, OldValue);
	// 스킬발동이 true가 되면 Sprint스킬실행.
	if (GetExecuteSprintSkill() > 0 && !bPlayerUseSprintSkill)
	{
		OnPlayerUseSkill.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_Sprint);
	}
	bPlayerUseSprintSkill = GetExecuteSprintSkill() > 0;
}
void UPlayerAttributeSet::OnRep_ExecuteOutLineSkill(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ExecuteOutLineSkill, OldValue);
	// 스킬발동이 true가 되면 OutLine스킬실행.
	if (GetExecuteOutLineSkill() > 0 && !bPlayerUseOutLineSkill)
	{
		OnPlayerUseSkill.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_OutLine);
	}
	bPlayerUseOutLineSkill = GetExecuteOutLineSkill() > 0;
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
		OnChangedSpeed.Broadcast(GetSpeed());
	}

	if (Data.EvaluatedData.Attribute == GetSpeedAdjustmentAttribute())
	{
		SetFinalSpeed(FMath::Clamp(GetSpeed() + GetSpeedAdjustment(), MinimumPoint, 10000));
	}
	// 체력이 30% 보다 늘어나면 Low HP 태그 제거.
	if (GetHP() >= GetMaxHP() * 0.3f && Data.Target.HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP))
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
	}
	// 체력이 MaxHp의 30%이하라면 Low HP 효과 발동.
	if (GetHP() < GetMaxHP() * 0.3f && !bPlayerLowHP)
	{
		OnPlayerLowHP.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
	}
	bPlayerLowHP = GetHP() < GetMaxHP() * 0.3f;
	// 체력이 0이하라면 다운.
	if (GetHP() <= 0.0f && !bPlayerDowned)
	{
		OnPlayerDowned.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
	}
	bPlayerDowned = GetHP() <= 0.0f;

	// 정신력이 MAX가 아니라면 거리별회복 GA 호출
	if (GetMentalPoint() < GetMaxMentalPoint() && !bMentalPointNotMax)
	{
		Data.Target.AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
		Data.Target.AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
		OnMentalPointNotMax.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
	}
	bMentalPointNotMax = GetMentalPoint() < GetMaxMentalPoint();
	// 정신력 이상 단계가 아니게 되면 태그 떼주기.
	if (GetMentalPoint() > 50.0f && Data.Target.HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st))
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
	}
	// 정신력이 50 이하라면 착란 1단계
	if (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f && !bPlayerConfused1st)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		OnPlayerConfused1st.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
	}
	bPlayerConfused1st = (GetMentalPoint() > 25.0f && GetMentalPoint() <= 50.0f);

	// 정신력이 25 이하라면 착란 2단계
	if (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f && !bPlayerConfused2nd)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st);
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
		OnPlayerConfused2nd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
	}
	bPlayerConfused2nd = (GetMentalPoint() > 0.0f && GetMentalPoint() <= 25.0f);

	// 정신력이 0 이라면 착란 3단계
	if (GetMentalPoint() <= 0.0f && !bPlayerConfused3rd)
	{
		Data.Target.RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		Data.Target.RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
		OnPlayerConfused3rd.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
	}
	bPlayerConfused3rd = GetMentalPoint() <= 0.0f;

	// 스킬발동이 true가 되면 Sprint스킬실행.
	if (GetExecuteSprintSkill() > 0 && !bPlayerUseSprintSkill)
	{
		OnPlayerUseSkill.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_Sprint);
	}
	bPlayerUseSprintSkill = GetExecuteSprintSkill() > 0;

	// 스킬발동이 true가 되면 OutLine스킬실행.
	if (GetExecuteOutLineSkill() > 0 && !bPlayerUseOutLineSkill)
	{
		OnPlayerUseSkill.Broadcast(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_OutLine);
	}
	bPlayerUseOutLineSkill = GetExecuteOutLineSkill() > 0;
}
