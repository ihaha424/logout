// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_UseItemSlot.h"

#include "Log/TPTLog.h"


UGA_UseItemSlot::UGA_UseItemSlot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_UseItemSlot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    NULLCHECK_RETURN_LOG(TriggerEventData, GALog, Error, );

    float SlotNumber = TriggerEventData->EventMagnitude;


    //TPT_LOG(GALog, Warning, TEXT(" %f"), SlotNumber);
    // 이 값을 이용해 아이템 사용 등 원하는 행동 수행
 
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
