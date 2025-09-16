// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "PlayerCharacter.h"
#include "PS_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Log/TPTLog.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UPlayerAnimInstance::UPlayerAnimInstance()
{
}

void UPlayerAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
	GameplayTagPropertyMap.Initialize(this, ASC);
}
#if WITH_EDITOR
EDataValidationResult UPlayerAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);
	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif
void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


	//for (auto& item: MontageInstances)
	//{
	//	if (!item || !item->Montage) continue;
	//	UKismetSystemLibrary::PrintString(this, item->Montage->GetName(), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 2, item->Montage->GetFName());
	//}

}
