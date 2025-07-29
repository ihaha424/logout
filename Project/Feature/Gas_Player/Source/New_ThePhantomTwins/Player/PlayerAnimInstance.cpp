// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAnimInstance.h"
#include "Player/PS_Player.h"
#include "Player/PlayerCharacter.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "AbilitySystemComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Owner = Cast<APlayerCharacter>(TryGetPawnOwner());
	NULLCHECK_RETURN_LOG(Owner, PlayerLog, Warning, )
	ASC = Owner->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Warning, )
	PS = Cast<APS_Player>(Owner->GetPlayerState());
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Warning, )
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	NULLCHECK_RETURN_LOG(Owner, PlayerLog, Warning, )
	Speed = Owner->GetVelocity().Length();

	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Warning, )
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Warning, )

	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	if (OwnedTags.HasTagExact(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed))
	{
		bIsGroggy = true;
		bIsRecovery = false;
	}
	else
	{
		bIsGroggy = false;
	}

	bIsRecovery = PS->IsRecovery();

	if (OwnedTags.HasTagExact(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch))
	{
		bIsCrouch = true;
	}
	else
	{
		bIsCrouch = false;
	}

	if (OwnedTags.HasTagExact(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact))
	{
		bIsInteractive = true;
		bIsInteract = true;
	}
	else
	{
		bIsInteractive = false;
		bIsInteract = false;
	}

	if (OwnedTags.HasTagExact(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill))
	{
		bIsActiveSkill = true;
	}
	else
	{
		bIsActiveSkill = false;
	}
}

void UPlayerAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}
