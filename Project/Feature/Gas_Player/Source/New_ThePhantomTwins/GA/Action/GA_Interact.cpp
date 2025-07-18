// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Player/FocusTraceComponent.h"
#include "SzInterface/Interact.h"

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Character->FocusTrace->GetFocusedActor();
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );
	// 플레이어가 상호작용할 수 있는 오브젝트가 있는지 확인
	if (TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		TPT_LOG(GALog, Log, TEXT("Active Interact GA"));

		if (IInteract::Execute_CanInteract(TargetActor, Character, true))
		{
			IInteract::Execute_OnInteractClient(TargetActor, Character);
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_Interact::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	TPT_LOG(GALog, Log, TEXT("Cancel Interact GA"));
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}
