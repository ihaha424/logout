// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_UseItemSlot.h"

#include "Log/TPTLog.h"


UGA_UseItemSlot::UGA_UseItemSlot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//TPT_LOG(GALog, Error, TEXT(""));
}

void UGA_UseItemSlot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	TPT_LOG(GALog, Warning, TEXT(" "));
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (TriggerEventData)
    {
        float SlotNumber = TriggerEventData->EventMagnitude;
        TPT_LOG(GALog, Warning, TEXT(" %f"), SlotNumber);
        // 이 값을 이용해 아이템 사용 등 원하는 행동 수행
    }
}