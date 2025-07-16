// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_OpenDoor.h"

#include "Tags/TPTGameplayTags.h"

UGA_OpenDoor::UGA_OpenDoor()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_OpenDoor);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_PerformingAction);
}

void UGA_OpenDoor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    AActor* Target = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;

    //if (Target && Target->Implements<UInteractableDoorInterface>())
    //{
    //    IInteractableDoorInterface::Execute_Open(Target, ActorInfo->AvatarActor.Get());
    //}

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
