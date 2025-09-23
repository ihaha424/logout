// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBaseAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UAIBaseAttributeSet::UAIBaseAttributeSet() {}

void UAIBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(FMath::Max(GetAttackPower(), 0.0f));
	}
}

void UAIBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, MentalityAttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, AttackCoolTime, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, SightRadius, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, LoseSightRadius, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, SightAngle, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, SightAge, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, HearingRange, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, HearingAge, COND_None, REPNOTIFY_Always);
	//DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, ImmediateDetectionRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAIBaseAttributeSet, ChaseMoveSpeed, COND_None, REPNOTIFY_Always);
}

void UAIBaseAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UAIBaseAttributeSet, AttackPower, OldValue); }
void UAIBaseAttributeSet::OnRep_MentalityAttackPower(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UAIBaseAttributeSet, MentalityAttackPower, OldValue); }
void UAIBaseAttributeSet::OnRep_AttackCoolTime(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UAIBaseAttributeSet, AttackCoolTime, OldValue);}
void UAIBaseAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UAIBaseAttributeSet, MoveSpeed, OldValue); }
void UAIBaseAttributeSet::OnRep_ChaseMoveSpeed(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UAIBaseAttributeSet, ChaseMoveSpeed, OldValue); }
