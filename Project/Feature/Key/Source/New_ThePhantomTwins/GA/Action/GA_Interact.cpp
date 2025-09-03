// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Log/TPTLog.h"
#include "Player/FocusTraceComponent.h"
#include "SzInterface/Interact.h"
#include "SzInterface/Holding.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/TPTGameplayTags.h"
#include "Player/PC_Player.h"

UGA_Interact::UGA_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact);
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	TargetActor = Cast<AActor>(Character->GetFocusTrace()->GetFocusedActor());
	NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	PlayInteractMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), InteractMontage, 1.0f);
	PlayInteractMontageTask->OnCompleted.AddDynamic(this, &UGA_Interact::OnCompleteCallback);

	PlayRecoveryMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("RecoveryMontage"), RecoveryMontage, 1.0f);
	PlayRecoveryMontageTask->OnCompleted.AddDynamic(this, &UGA_Interact::OnCompleteCallback);

	FTimerDelegate TimerDel;
	float Time = 0.0f;

	if (TargetActor->GetClass()->ImplementsInterface(UHolding::StaticClass())) // 플레이어 오브젝트
	{
		Time = IHolding::Execute_GetTime(TargetActor);
		IHolding::Execute_SetHoldingGaugeUI(TargetActor, Character, true);

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
	}

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
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

void UGA_Interact::OnCompleteCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Interact::InteractExecute()
{
	NULLCHECK_RETURN_LOG(TargetActor, GALog, Warning, );
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	TargetActor->GetWorld()->GetTimerManager().ClearTimer(CompleteHandle);
	TargetActor->GetWorld()->GetTimerManager().ClearTimer(UpdateHandle);

	// 플레이어가 상호작용할 수 있는 오브젝트가 있는지 확인
	if (TargetActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
	{
		if (Character->HasAuthority())
		{
			C2S_Interact(TargetActor, Character);
		}
		IInteract::Execute_OnInteractClient(TargetActor, Character);

		// 몽타주 골라서 재생
		if (Cast<APlayerCharacter>(TargetActor))
		{
			TPT_LOG(GALog, Warning, TEXT("RecoveryMontage"));
			PlayRecoveryMontageTask->ReadyForActivation();
			// TODO : 몽타주 재생시에 움직임이 없으면 풀바디 재생이 되도록 조정하기.
			// TODO : 릴리즈드가 되면 재생도 멈추게 조정하기.
		}
		else
		{
			PlayInteractMontageTask->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}