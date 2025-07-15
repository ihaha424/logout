// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "../Player/PS_Player.h"
#include "../Attribute/PlayerAttributeSet.h"

UGA_Run::UGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaDrainEffect, GetAbilityLevel());
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	}


	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		APS_Player* PS = Cast<APS_Player>(Character->GetPlayerState());

		if (PS)
		{
			//Character->GetCharacterMovement()->MaxWalkSpeed = PS->AttributeSet->Speed;
		}
	}

    // ¸ùÅ¸ÁÖ Àç»ý
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
