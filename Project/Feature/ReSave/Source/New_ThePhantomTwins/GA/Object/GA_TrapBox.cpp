// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Object/GA_TrapBox.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"

UGA_TrapBox::UGA_TrapBox()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_OpenTrapBox);
	SetAssetTags(DefaultTags);
}

void UGA_TrapBox::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, 0.3f);
    WaitTask->OnFinish.AddDynamic(this, &UGA_TrapBox::OnDelayFinished);
    WaitTask->ReadyForActivation();
}

void UGA_TrapBox::OnDelayFinished()
{
    UAbilityTask_PlayMontageAndWait* PlayTask =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, TEXT("TrapBoxMontage"), OpenTrapBoxMontage, 1.0f);

    PlayTask->OnCompleted.AddDynamic(this, &UGA_TrapBox::OnMontageComplete);
    PlayTask->OnInterrupted.AddDynamic(this, &UGA_TrapBox::OnMontageComplete);
    PlayTask->OnCancelled.AddDynamic(this, &UGA_TrapBox::OnMontageComplete);

    PlayTask->ReadyForActivation();
}

void UGA_TrapBox::OnMontageComplete()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}