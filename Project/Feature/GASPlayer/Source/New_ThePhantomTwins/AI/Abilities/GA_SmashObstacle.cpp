// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SmashObstacle.h"
#include "SzInterface/Destroyable.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"

UGA_SmashObstacle::UGA_SmashObstacle()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

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
    AActor* Target = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;
    if (Target && Target->GetClass()->ImplementsInterface(UDestroyable::StaticClass()))
    {
        if (IDestroyable::Execute_CanBeDestroyed(Target, Cast<APawn>(ActorInfo->AvatarActor.Get())))
        {
            IDestroyable::Execute_OnDestroy(Target, Cast<APawn>(ActorInfo->AvatarActor.Get()));
        }
    }
    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
        MyASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }

    // 애니메이션 & VFX(서버 클라 둘다)
    // 애니메이션 & VFX가 끝나면 EndAbillity


    // 위 과업을 하기 전까지 사용할 테스트용 코드 1초후 종료
    FTimerHandle TimerHandle;
    FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UGA_SmashObstacle::EndAbility,
        Handle, ActorInfo, ActivationInfo, true, false);
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(TimerHandle, EndDelegate, 1.0f, false);
    }
}

void UGA_SmashObstacle::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo())
    {
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
        MyASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
    }
}
