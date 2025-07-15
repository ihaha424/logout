// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../Player/PS_Player.h"
#include "../Attribute/PlayerAttributeSet.h"

UGA_Run::UGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectSpecHandle StaminaDrainEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaDrainEffect, GetAbilityLevel());
	if (StaminaDrainEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaDrainEffectSpecHandle);
	}

	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SetSpeedEffectSpecHandle);
	}

	FTimerHandle TimerHandle;
	ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
		TimerHandle, [this, ActorInfo]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
			if (!Character) return;

			const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();

			float Speed = Attribute->GetFinalSpeed();

			Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}, 0.05f, false); // 0.05초 정도 후에 반영

    // 몽타주 재생
	//if (RunningMontage)
	//{
	//    PlayMontage(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), RunningMontage);
	//}
}

void UGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	OnRunCallback();
}

void UGA_Run::OnRunCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
