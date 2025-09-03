// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Crouch.h"

// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_Crouch::UGA_Crouch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	//if (Character->HasAuthority())
	//{
	//	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	//	NULLCHECK_RETURN_LOG(ASC, GALog, Warning, );
	//	ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
	//	ASC->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
	//}

	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, CrouchSpeed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}

	FGameplayEffectSpecHandle TagEffectSpecHandle = MakeOutgoingGameplayEffectSpec(TagEffect, 1.0f);
	if (TagEffectSpecHandle.IsValid())
	{
		ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TagEffectSpecHandle);
	}

	const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
	float Speed = Attribute->GetFinalSpeed();
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = Speed;
	Character->Crouch();
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	//if (Character->HasAuthority())
	//{
	//	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	//	NULLCHECK_RETURN_LOG(ASC, GALog, Warning, );
	//	ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
	//	ASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
	//}

	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, Character->WalkSpeed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (ActiveEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
	}

	const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
	float Speed = Attribute->GetFinalSpeed();
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	Character->UnCrouch();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crouch::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
