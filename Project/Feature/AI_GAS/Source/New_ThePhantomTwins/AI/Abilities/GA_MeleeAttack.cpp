// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MeleeAttack.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    AActor* Target = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;

    // TODO: 데미지 계산 또는 어플리케이션 로직
    if (Target)
    {
        // 예: 데미지 적용 또는 태그 부여 등
        UE_LOG(LogTemp, Log, TEXT("Melee Attack Target: %s"), *Target->GetName());
    }

    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }


    // 애니메이션 재생 예시
    if (AttackMontage && ActorInfo->AvatarActor.IsValid())
    {
        UAnimInstance* AnimInstance = Cast<ACharacter>(ActorInfo->AvatarActor.Get())->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }

    // 위 과업을 하기 전까지 사용할 테스트용 코드 1초후 종료
    FTimerHandle TimerHandle;
    FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UGA_MeleeAttack::EndAbility,
        Handle, ActorInfo, ActivationInfo, true, false);
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TimerHandle, EndDelegate, 1.0f, false);
    }
}

void UGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }
}
