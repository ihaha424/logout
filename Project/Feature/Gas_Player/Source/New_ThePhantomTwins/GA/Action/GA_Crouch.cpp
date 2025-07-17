// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Crouch.h"

// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

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

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

	const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
	float Speed = Attribute->GetFinalSpeed();

	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = Speed;

	// 현재 상태에 따라 반대로 토글
	if (Character->bIsCrouched)
	{
		Character->UnCrouch();  // 서기

		if (SetSpeedEffectSpecHandle.IsValid())
		{
			FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
			SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, Character->WalkSpeed);
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
		}

		float WalkSpeed = Attribute->GetFinalSpeed();

		Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	}
	else
	{
		Character->Crouch();    // 웅크리기
	}

	// 몽타주 재생
	//if (CrouchingMontage)
	//{
	//    PlayMontage(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), RunningMontage);
	//}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Crouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

	Character->UnCrouch();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
