// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Abilities/GA_PlayMontage.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/NamePayload.h"
#include "Log/TPTLog.h"

UGA_PlayMontage::UGA_PlayMontage()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
    bRetriggerInstancedAbility = true;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    NULLCHECK_RETURN_LOG(TriggerEventData, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false););
    AActor* Target = const_cast<AActor*>(TriggerEventData->Target.Get());
    FName MontageSectionName = NAME_None;
    if (TriggerEventData->OptionalObject)
    {
        const UNamePayload* Payload = Cast<UNamePayload>(TriggerEventData->OptionalObject);
        if (Payload)
        {
            MontageSectionName = Payload->Name;
        }
    }

    NULLCHECK_RETURN_LOG(AnimationMontage, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false););
    if (!ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (StopCurrentOrJumpIfSame(ActorInfo, AnimationMontage, MontageSectionName))
    {
        return;
    }

    UAbilityTask_PlayMontageAndWait* PlayTask =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            TEXT("PlayMontage"),
            AnimationMontage,
            1.0f,
            MontageSectionName);
    NULLCHECK_RETURN_LOG(PlayTask, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false););

    PlayTask->OnCompleted.AddDynamic(this, &UGA_PlayMontage::OnCompleteCallback);
    PlayTask->OnBlendOut.AddDynamic(this, &UGA_PlayMontage::OnCompleteCallback);
    PlayTask->OnInterrupted.AddDynamic(this, &UGA_PlayMontage::OnInterruptedCallback);
    PlayTask->OnCancelled.AddDynamic(this, &UGA_PlayMontage::OnInterruptedCallback);
    // 기존에 있는 섹션이 바뀌면 어떤 델리게이트가 호출되나?

    PlayTask->ReadyForActivation();
}

void UGA_PlayMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo)
{

    EndAbility(Handle, ActorInfo, ActivationInfo, true, bInterrupted);
}

void UGA_PlayMontage::OnCompleteCallback()
{
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = false; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PlayMontage::OnInterruptedCallback()
{
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = true; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_PlayMontage::StopCurrentOrJumpIfSame(const FGameplayAbilityActorInfo* ActorInfo, UAnimMontage* Montage, const FName SectionToJump, float StopBlendOut)
{
    if (!ActorInfo || !Montage) return false;

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    UAnimInstance* Anim = ActorInfo->GetAnimInstance();
    NULLCHECK_RETURN_LOG(Anim, AILog, Warning, false);


    // 1) 이미 같은 몽타주가 재생 중이면 → 섹션 점프만
    const bool bSameByASC = (ASC && ASC->GetCurrentMontage() == Montage);
    const bool bSameByAnim = Anim->Montage_IsPlaying(Montage);

    // 재사용시 callback이 안됨.
    //if (bSameByASC || bSameByAnim)
    //{
    //    if (SectionToJump != NAME_None)
    //    {
    //        Anim->Montage_JumpToSection(SectionToJump, Montage);
    //    }
    //    return true; // 여기서 처리 완료(새 재생 불필요)
    //}

    // 2) 다른 몽타주가 재생 중이면 → 정지
    if (ASC && ASC->GetCurrentMontage())
    {
        ASC->CurrentMontageStop(StopBlendOut);
    }
    else if (Anim->IsAnyMontagePlaying())
    {
        Anim->Montage_Stop(StopBlendOut);
    }

    return false; // 새로 재생해야 함
}
