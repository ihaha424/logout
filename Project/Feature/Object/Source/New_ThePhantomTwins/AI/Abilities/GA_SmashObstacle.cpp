// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SmashObstacle.h"
#include "SzInterface/Destroyable.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

#include "Log/TPTLog.h"


UGA_SmashObstacle::UGA_SmashObstacle()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_SmashObstacle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    NULLCHECK_CODE_RETURN_LOG(TriggerEventData, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    Target = const_cast<AActor*>(TriggerEventData->Target.Get());
    Count = TriggerEventData->EventMagnitude;
    CurCount = 0;
    if (Target && Target->GetClass()->ImplementsInterface(UDestroyable::StaticClass()))
    {
        AAIBaseCharacter* Owner = Cast<AAIBaseCharacter>(ActorInfo->AvatarActor.Get());
        if (IsValid(Owner) && IDestroyable::Execute_CanBeDestroyed(Target, Owner))
        {
            bActiveAbility = true;
            OwnerPawn = Owner;

            if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
            {
                MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
                MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
            }

            if (CurCount < Count)
            {
                UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                    this, TEXT("PlayAttack"), AttackMontage, 1.0f, TEXT("Attack"));
                NULLCHECK_CODE_RETURN_LOG(PlayAttackTask, AILog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
                PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_SmashObstacle::OnCompleteCallback);
                PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_SmashObstacle::OnInterruptedCallback);
                PlayAttackTask->ReadyForActivation();
                return;
            }

            if (IsValid(OwnerPawn) && IsValid(Target))
            {
                IDestroyable::Execute_OnDestroy(Target, OwnerPawn);
            }
        }
    }
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_SmashObstacle::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
    if (bActiveAbility && MyASC)
    {
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }

    if (OwnerPawn)
    {
        AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
        if (AIController)
        {
            EPathFollowingStatus::Type Status = AIController->GetPathFollowingComponent()->GetStatus();
            AIController->StopMovement();
        }
    }
    bActiveAbility = false;
    Target = nullptr;
    OwnerPawn = nullptr;
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SmashObstacle::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo)
{
    EndAbility(Handle, ActorInfo, ActivationInfo, true, bInterrupted);
}

void UGA_SmashObstacle::OnCompleteCallback()
{
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = false; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    CurCount++;
    if (CurCount < Count)
    {
        UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, TEXT("PlayAttack"), AttackMontage, 1.0f, TEXT("Attack"));
        NULLCHECK_CODE_RETURN_LOG(PlayAttackTask, AILog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);, );
        PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_SmashObstacle::OnCompleteCallback);
        PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_SmashObstacle::OnInterruptedCallback);
        PlayAttackTask->ReadyForActivation();
        return;
    }

    if (IsValid(OwnerPawn) && IsValid(Target))
    {
        IDestroyable::Execute_OnDestroy(Target, OwnerPawn);
    }
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SmashObstacle::OnInterruptedCallback()
{
    bool bReplicateEndAbility = true; // 서버에서 실행되는 어빌리티는 클라이언트에게도 복제되어야 한다.
    bool bWasCancelled = true; // 몽타주가 끝나면 취소되지 않았으므로 false로 설정한다.
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
