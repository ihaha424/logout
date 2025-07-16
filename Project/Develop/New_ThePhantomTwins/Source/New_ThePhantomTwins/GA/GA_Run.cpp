// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../Player/PS_Player.h"
#include "../Attribute/PlayerAttributeSet.h"
#include "../Tags/TPTGameplayTags.h"
#include "../Log/TPTLog.h"

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
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, RunSpeed);
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


	// ActivateAbility 안에서
	GetWorld()->GetTimerManager().SetTimer(StaminaCheckHandle, [this]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
			if (!Character) return;

			const UPlayerAttributeSet* Attr = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
			if (Attr->GetStamina() <= 0.f)
			{
				// 스태미나 0이면 종료
				CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			}
		}, 0.1f, true); // 0.1초마다 체크

    // 몽타주 재생
	//if (RunningMontage)
	//{
	//    PlayMontage(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), RunningMontage);
	//}
}

void UGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
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
		Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

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
		Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
