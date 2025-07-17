// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_Run::UGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CancelTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffect, MyASC);

	MyASC->CancelAbilities(&CancelTags);

	// 스태미너 감소 GE 부여
	FGameplayEffectSpecHandle StaminaDrainEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaDrainEffect, GetAbilityLevel());
	if (StaminaDrainEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaDrainEffectSpecHandle);
	}

	// 스피드 재정의 GE 부여
	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, RunSpeed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}

	// 재정의된 스피드 적용
	FTimerHandle TimerHandle;
	ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
		TimerHandle, [this, ActorInfo]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
			NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

			const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();

			float Speed = Attribute->GetFinalSpeed();

			Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}, 0.05f, false); // 0.05초 정도 후에 반영


	// 스태미너가 0이되면 강제 중지
	GetWorld()->GetTimerManager().SetTimer(StaminaCheckHandle, [this]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
			NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

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
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	// 스태미너 감소 GE 제거
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaDrainEffect, MyASC);

	// 스태미너 재생 GE 부여
	FGameplayEffectSpecHandle StaminaRegenEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaRegenEffect, GetAbilityLevel());
	if (StaminaRegenEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaRegenEffectSpecHandle);
	}

	// 스피드 재정의 GE 부여 & 재정의된 스피드 적용
	FGameplayEffectSpecHandle WalkSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (WalkSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
		NULLCHECK_RETURN_LOG(Character, GALog, Warning, )
		

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
