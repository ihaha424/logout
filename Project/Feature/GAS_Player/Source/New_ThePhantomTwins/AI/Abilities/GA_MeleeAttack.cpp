// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MeleeAttack.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat);
    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AActor* Target = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;

    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
        MyASC->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }

    NULLCHECK_CODE_RETURN_LOG(AttackMontage, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    if (ActorInfo->AvatarActor.IsValid())
    {
        AAIBaseCharacter* Character = Cast<AAIBaseCharacter>(ActorInfo->AvatarActor.Get());
        NULLCHECK_CODE_RETURN_LOG(Character, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
        Character->SetAttackCollision(true);

        // 몽타주 재생을 위해서는 어빌리티 태스크(시간소요관리, 상태관리를 비동기적으로 처리할수있음)를 이용.
        UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, TEXT("PlayAttack"), AttackMontage, 1.0f, TEXT("Attack")); // 이 어빌리티를 소유할 곳, 이 어빌리티의 이름, 캐릭터에서 제공받을 몽타주 에셋, 시작섹션
        NULLCHECK_CODE_RETURN_LOG(PlayAttackTask, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
        PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnCompleteCallback);
        PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnInterruptedCallback);
        PlayAttackTask->ReadyForActivation();
        return;
    }

    // 애니메이션 재생 실패 시 Fallback 처리 (즉시 종료)
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
        MyASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }

    AAIBaseCharacter* Character = Cast<AAIBaseCharacter>(ActorInfo->AvatarActor.Get());
    NULLCHECK_CODE_RETURN_LOG(Character, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    Character->SetAttackCollision(true);
}

void UGA_MeleeAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo)
{
    EndAbility(Handle, ActorInfo, ActivationInfo, true, bInterrupted);
}

void UGA_MeleeAttack::OnCompleteCallback()
{
    // 어빌리티가 끝나면   EndAbility를 호출한다.
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = false; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeAttack::OnInterruptedCallback()
{
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = true; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
