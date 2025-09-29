// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Object/GA_TrapBox.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_TrapBox::UGA_TrapBox()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_TrapBox::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );

	TPT_LOG(GALog, Log, TEXT("UGA_TrapBox::ActivateAbility"));

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

	//UAbilityTask_PlayMontageAndWait* PlayTrapBoxMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("TapBoxMontage"), TapBoxMontage, 1.0f);
	//PlayTrapBoxMontageTask->OnCompleted.AddDynamic(this, &UGA_TrapBox::OnMontageComplete);

	//PlayTrapBoxMontageTask->ReadyForActivation();
}

void UGA_TrapBox::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
