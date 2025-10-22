// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Log/TPTLog.h"
#include "Player/FocusTraceComponent.h"
#include "SzInterface/Interact.h"
#include "SzInterface/Holding.h"
#include "Tags/TPTGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact);
	SetAssetTags(DefaultTags);
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	TargetActor = Cast<AActor>(Character->GetFocusTrace()->GetFocusedActor());
	NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	// 사물 상호작용
	PlayInteractMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), InteractMontage, 1.0f);
	PlayInteractMontageTask->OnCompleted.AddDynamic(this, &UGA_Interact::OnMontageComplete);
	PlayInteractMontageTask->OnInterrupted.AddDynamic(this, &UGA_Interact::OnMontageComplete);
	// 회복 상호작용
	PlayRecoveryMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("RecoveryMontage"), RecoveryMontage, 1.0f);

	if (TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		// 몽타주 골라서 재생
		if (Cast<APlayerCharacter>(TargetActor))
		{
			CurrentPlayingMontage = RecoveryMontage;
			PlayRecoveryMontageTask->ReadyForActivation();
		}
		else
		{
			CurrentPlayingMontage = InteractMontage;
			PlayInteractMontageTask->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	if (TargetActor->GetClass()->ImplementsInterface(UHolding::StaticClass())) // 플레이어 오브젝트
	{
		//TPT_LOG(GALog, Error, TEXT("TEST__ ImplementsInterface"));
		float Time = IHolding::Execute_GetTime(TargetActor);
		IHolding::Execute_SetHoldingGaugeUI(TargetActor, Character, true);

		if (SoundCue && (!ActiveAudioComponent || ActiveAudioComponent->IsPlaying())) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent = UGameplayStatics::SpawnSoundAttached(SoundCue, Character->GetRootComponent());
		}

		FTimerDelegate TimerDel;
		TimerDel.BindLambda([this, Time]()
			{
				if (GetWorld()->GetTimerManager().IsTimerActive(CompleteHandle))
				{
					float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(CompleteHandle);
					IHolding::Execute_CalculateGaugePercent(TargetActor, Elapsed);
				}
			});

		TargetActor->GetWorld()->GetTimerManager().SetTimer(
			UpdateHandle,
			TimerDel,
			0.02f,
			true
		);

		TargetActor->GetWorld()->GetTimerManager().SetTimer(
			CompleteHandle,
			this,
			&UGA_Interact::InteractExecute,
			Time,
			false
		);
	}
	else
	{
		InteractExecute();
	}
}

void UGA_Interact::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ActiveAudioComponent)
	{
		ActiveAudioComponent->Stop();
		ActiveAudioComponent = nullptr;
	}
}

void UGA_Interact::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, )
	NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, CancelAbility(Handle, ActorInfo, ActivationInfo, true);, )

	if (TargetActor->GetClass()->ImplementsInterface(UHolding::StaticClass()))
	{
		TargetActor->GetWorld()->GetTimerManager().ClearTimer(CompleteHandle);
		TargetActor->GetWorld()->GetTimerManager().ClearTimer(UpdateHandle);

		IHolding::Execute_SetHoldingGaugeUI(TargetActor, Character, false);
		IHolding::Execute_CalculateGaugePercent(TargetActor, 0.0f);

		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGA_Interact::C2S_Interact_Implementation(UObject* interact, AActor* Owner)
{
	const APlayerCharacter* OwnerCharacter = Cast<APlayerCharacter>(Owner);
	NULLCHECK_RETURN_LOG(OwnerCharacter, GALog, Warning, );

	if (interact != nullptr && interact->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		IInteract::Execute_OnInteractServer(interact, OwnerCharacter);
	}
}

void UGA_Interact::InteractExecute()
{
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	if (TargetActor->GetClass()->ImplementsInterface(UHolding::StaticClass()))
	{
		TargetActor->GetWorld()->GetTimerManager().ClearTimer(CompleteHandle);
		TargetActor->GetWorld()->GetTimerManager().ClearTimer(UpdateHandle);
	}
	if (TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		if (Character->HasAuthority())
		{
			C2S_Interact(TargetActor, Character);
		}

		IInteract::Execute_OnInteractClient(TargetActor, Character);

		if (CurrentPlayingMontage == RecoveryMontage)
		{
			Character->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, RecoveryMontage);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}