// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"
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
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact);
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Cast<AActor>(Character->FocusTrace->GetFocusedActor());

	// 플레이어가 상호작용할 수 있는 오브젝트가 있는지 확인
	if (TargetActor != nullptr && TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		C2S_Interact(TargetActor, Character);
		IInteract::Execute_OnInteractClient(TargetActor, Character);
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
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	AActor* TargetActor = Character->FocusTrace->GetFocusedActor();
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );

	if (APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(TargetActor))
	{
		OtherPlayer->GetWorld()->GetTimerManager().ClearTimer(OtherPlayer->RecoveryTimerHandle);
		
		APC_Player* PC = APC_Player::GetLocalPlayerController(Character);
		PC->SetWidget(TEXT("RecoveryGauge"), false, EMessageTargetType::Multicast);
	}
}

void UGA_Interact::C2S_Interact_Implementation(UObject* interact, AActor* Owner)
{
	const APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	UKismetSystemLibrary::PrintString(this, FString("C2S_Interact"));

	if (interact != nullptr && interact->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		IInteract::Execute_OnInteractServer(interact, Character);
	}
}