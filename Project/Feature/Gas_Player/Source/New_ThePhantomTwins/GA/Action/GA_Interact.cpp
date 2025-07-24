// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Player/FocusTraceComponent.h"
#include "SzInterface/Interact.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//TPT_LOG(GALog, Log, TEXT("Activate Interact Ability"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Character->FocusTrace->GetFocusedActor();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(TargetActor))
	{
		if (Player->GetAbilitySystemComponent()->HasMatchingGameplayTag
		(FTPTGameplayTags::Get().TPTGameplay_Character_State_Recovery))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}

	// 플레이어가 상호작용할 수 있는 오브젝트가 있는지 확인
	if (TargetActor != nullptr && TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		C2S_Interact(TargetActor, Character);
		IInteract::Execute_OnInteractClient(TargetActor, Character);
		//TPT_LOG(GALog, Log, TEXT("OnInteract Client"));
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_Interact::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	//TPT_LOG(GALog, Error, TEXT("Cancel Interact GA"));
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Character->FocusTrace->GetFocusedActor();
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );

	if (APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(TargetActor))
	{
		OtherPlayer->GetWorld()->GetTimerManager().ClearTimer(OtherPlayer->RecoveryTimerHandle);
	}
}

void UGA_Interact::C2S_Interact_Implementation(UObject* interact, AActor* Owner)
{
	const APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	if (interact != nullptr && interact->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		//TPT_LOG(GALog, Log, TEXT("OnInteract Server"));
		IInteract::Execute_OnInteractServer(interact, Character);
	}
}