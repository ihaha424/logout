// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Log/TPTLog.h"
#include "Player/FocusTraceComponent.h"
#include "SzInterface/Interact.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/TPTGameplayTags.h"
#include "Player/PC_Player.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	//ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact);
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	AActor* TargetActor = Cast<AActor>(Character->GetFocusTrace()->GetFocusedActor());
	NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), InteractMontage, 1.0f);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Interact::OnCompleteCallback);
	
	// 플레이어가 상호작용할 수 있는 오브젝트가 있는지 확인
	if (TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		if (Character->HasAuthority())
		{
			C2S_Interact(TargetActor, Character);
		}
		IInteract::Execute_OnInteractClient(TargetActor, Character);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_Interact::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Character->GetFocusTrace()->GetFocusedActor();
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );

	if (APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(TargetActor))
	{
		OtherPlayer->GetWorld()->GetTimerManager().ClearTimer(OtherPlayer->RecoveryTimerHandle);
		OtherPlayer->GetWorld()->GetTimerManager().ClearTimer(OtherPlayer->TempHandle);

		APC_Player* PC = APC_Player::GetLocalPlayerController(Character);
		PC->SetWidget(TEXT("RecoveryGauge"), false, EMessageTargetType::Multicast);
	}
}
void UGA_Interact::C2S_Interact_Implementation(UObject* interact, AActor* Owner)
{
	const APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	if (interact != nullptr && interact->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		IInteract::Execute_OnInteractServer(interact, Character);
	}
}

void UGA_Interact::OnCompleteCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
