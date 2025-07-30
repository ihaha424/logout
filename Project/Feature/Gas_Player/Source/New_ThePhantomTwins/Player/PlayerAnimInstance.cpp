// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "PlayerCharacter.h"
#include "PS_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Log/TPTLog.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UPlayerAnimInstance::UPlayerAnimInstance()
{
}

void UPlayerAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
	TPT_LOG(PlayerLog, Error, TEXT("22"));

	GameplayTagPropertyMap.Initialize(this, ASC);
}

EDataValidationResult UPlayerAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	TPT_LOG(PlayerLog, Error, TEXT("11"));
	const APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwningActor());
	NULLCHECK_RETURN_LOG(Character, PlayerLog, Error, );
	TPT_LOG(PlayerLog, Error, TEXT("1331"));

	APS_Player* PS = Character->GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	TPT_LOG(PlayerLog, Error, TEXT("1441"));
	//InitializeWithAbilitySystem(ASC);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	const APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwningActor());
	NULLCHECK_RETURN_LOG(Character, PlayerLog, Error, );

}
