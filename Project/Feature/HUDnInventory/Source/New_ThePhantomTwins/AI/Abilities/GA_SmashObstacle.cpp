// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SmashObstacle.h"

#include "Tags/TPTGameplayTags.h"

UGA_SmashObstacle::UGA_SmashObstacle()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_SmashObstacle);
    ActivationOwnedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat);
}

void UGA_SmashObstacle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    AActor* Target = TriggerEventData ? const_cast<AActor*>(TriggerEventData->Target.Get()) : nullptr;

    //if (Target && Target->Implements<UInteractableObstacleInterface>())
    //{
    //    IInteractableObstacleInterface::Execute_Break(Target, ActorInfo->AvatarActor.Get());
    //}

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
