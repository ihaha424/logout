// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ActiveSkill/GA_RandomSkill.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Player/PC_Player.h"
#include "Log/TPTLog.h"

UGA_RandomSkill::UGA_RandomSkill()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
   // AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_RandomActive);
}

void UGA_RandomSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    const APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
    NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , )
        UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    NULLCHECK_CODE_RETURN_LOG(ASC, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , )

        TSubclassOf<UGameplayEffect> SelectedEffectClass;

    // 랜덤 숫자 찾기
    int32 RandomNumber = FMath::RandRange(1, 6);
    switch (RandomNumber)
    {
    case 1:	// 스프린트
        SelectedEffectClass = SprintEffectClass;
        TPT_LOG(GALog, Log, TEXT("SelectedSkill : Sprint"));
        break;
    case 2:	// 튼튼이
        SelectedEffectClass = HPBuffEffectClass;
        TPT_LOG(GALog, Log, TEXT("SelectedSkill : HPBuff"));
        break;
    case 3:	// 날쌘이
        SelectedEffectClass = StaminaBuffEffectClass;
        TPT_LOG(GALog, Log, TEXT("SelectedSkill : StaminaBuff"));
        break;
    case 4:	// 단단이
        SelectedEffectClass = MentalBuffEffectClass;
        TPT_LOG(GALog, Log, TEXT("SelectedSkill : MentalBuff"));
        break;
    case 5:	// 오라보기 // 추후수정
        SelectedEffectClass = AuraEffectClass;
        TPT_LOG(GALog, Log, TEXT("SelectedSkill : Aura"));
        break;
    default:
        break;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(SelectedEffectClass, 1.0f);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectSpecHandle);
    }

    FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
    if (CoolDownSpecHandle.IsValid())
    {
        CoolDownSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, CoolDownValue);
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}