// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_Sprint::UGA_Sprint()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_Sprint);
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    FGameplayTag SprintingTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Sprinting;
    TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = ASC->GetActiveEffectsWithAllTags(FGameplayTagContainer(SprintingTag));
    float RemainingDuration = 0.f;

    FActiveGameplayEffectHandle OldEffectHandle;
    if (ActiveEffectHandles.Num() > 0)
    {
        // 가장 첫 번째 활성화된 에너지드링크 효과 취득 (복수 처리도 가능)
        OldEffectHandle = ActiveEffectHandles[0];

        const FActiveGameplayEffect* ActiveEffect = ASC->GetActiveGameplayEffect(OldEffectHandle);
        if (ActiveEffect)
        {
            float StartTime = ActiveEffect->StartWorldTime;
            float Duration = ActiveEffect->Spec.GetDuration();
            float CurrentTime = GetWorld()->GetTimeSeconds();

            float ElapsedTime = CurrentTime - StartTime;
            RemainingDuration = FMath::Max(0.f, Duration - ElapsedTime);
        }

        ASC->RemoveActiveGameplayEffect(OldEffectHandle);
    }

    // 4. 새 Duration 계산 (기존 남은 시간 + 새 효과 시간)
    float TotalDuration = RemainingDuration + SprintDuration;
    NULLCHECK_CODE_RETURN_LOG(SprintEffect, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
	FGameplayEffectSpecHandle SprintSpecHandle = ASC->MakeOutgoingSpec(SprintEffect, GetAbilityLevel(), ASC->MakeEffectContext());
    if (SprintSpecHandle.IsValid())
    {
        SprintSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, TotalDuration);
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SprintSpecHandle);
    }

    FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
    if (CoolDownSpecHandle.IsValid())
    {
        CoolDownSpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_CoolDownCount, CoolDownValue);
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
