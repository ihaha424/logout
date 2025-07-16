// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Crouch.h"

// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../Player/PS_Player.h"
#include "../Attribute/PlayerAttributeSet.h"
#include "../Tags/TPTGameplayTags.h"
#include "../Log/TPTLog.h"

UGA_Crouch::UGA_Crouch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, CrouchSpeed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}

	FTimerHandle TimerHandle;
	ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
		TimerHandle, [this, ActorInfo]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
			if (!Character) return;

			const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();

			float Speed = Attribute->GetFinalSpeed();

			Character->GetCharacterMovement()->MaxWalkSpeedCrouched = Speed;
			Character->bIsCrouched = true;
			Character->Crouch();
		}, 0.05f, false); // 0.05초 정도 후에 반영

	// 몽타주 재생
	//if (CrouchingMontage)
	//{
	//    PlayMontage(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), RunningMontage);
	//}
}

void UGA_Crouch::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	FGameplayEffectSpecHandle WalkSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (WalkSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());

		if (!Character)
		{
			TPT_LOG(GELog, Log, TEXT("Fail Find Character!"));
			return;
		}

		WalkSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, Character->WalkSpeed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, WalkSpeedEffectSpecHandle);

		const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
		float Speed = Attribute->GetFinalSpeed();
		Character->bIsCrouched = false;
		Character->UnCrouch();
	}
}
