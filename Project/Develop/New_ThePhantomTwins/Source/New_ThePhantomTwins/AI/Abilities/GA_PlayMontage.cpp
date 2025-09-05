// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Abilities/GA_PlayMontage.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Data/NamePayload.h"
#include "Log/TPTLog.h"

UGA_PlayMontage::UGA_PlayMontage()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    NULLCHECK_CODE_RETURN_LOG(TriggerEventData, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
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
    

    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
        MyASC->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
    }

    NULLCHECK_CODE_RETURN_LOG(AnimationMontage, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    if (ActorInfo->AvatarActor.IsValid())
    {
        // 몽타주 재생을 위해서는 어빌리티 태스크(시간소요관리, 상태관리를 비동기적으로 처리할수있음)를 이용.
        UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, TEXT("PlayAttack"), AnimationMontage, 1.0f, MontageSectionName); // 이 어빌리티를 소유할 곳, 이 어빌리티의 이름, 캐릭터에서 제공받을 몽타주 에셋, 시작섹션
        NULLCHECK_CODE_RETURN_LOG(PlayAttackTask, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
        PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_PlayMontage::OnCompleteCallback);
        PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_PlayMontage::OnInterruptedCallback);
        PlayAttackTask->ReadyForActivation();
        return;
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PlayMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
        MyASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_Animation);
    }
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
