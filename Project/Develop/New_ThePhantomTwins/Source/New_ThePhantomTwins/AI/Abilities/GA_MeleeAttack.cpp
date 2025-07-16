// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MeleeAttack.h"
#include "GameFramework/Character.h"

#include "Tags/TPTGameplayTags.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_MeleeAttack);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat);
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

    // 애니메이션 재생 예시
    if (AttackMontage && ActorInfo->AvatarActor.IsValid())
    {
        UAnimInstance* AnimInstance = Cast<ACharacter>(ActorInfo->AvatarActor.Get())->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
