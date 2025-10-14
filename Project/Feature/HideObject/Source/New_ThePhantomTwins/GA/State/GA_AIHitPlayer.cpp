// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AIHitPlayer.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Player/PlayerCharacter.h"
#include "TimerManager.h"
#include "Components/PostProcessComponent.h"

UGA_AIHitPlayer::UGA_AIHitPlayer()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
	SetAssetTags(DefaultTags);
}

void UGA_AIHitPlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, );
	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st).AddUObject(this, &ThisClass::OffSound);
	
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());

	if (Character && Character->IsLocallyControlled())
	{
		if (USoundBase* Sound = SoundCue) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, Character->GetRootComponent());
		}

		PPComp = Character->FindComponentByClass<UPostProcessComponent>();
		if (PPComp && PPComp->Settings.WeightedBlendables.Array.Num() > 0)
		{
			// 즉시 효과 켜기
			FPostProcessSettings NewSettings = PPComp->Settings;
			NewSettings.WeightedBlendables.Array[0].Weight = 0.5f;
			PPComp->Settings = NewSettings;

			// 0.1초 후에 자동으로 끄기
			FTimerHandle TempHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TempHandle, this, &UGA_AIHitPlayer::FadeOutHitEffect, 0.3f, false);
		}
	}

	HitMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HitMontage"), HitMontage, 1.0f);
	HitMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);
	HitMontageTask->ReadyForActivation();
}

void UGA_AIHitPlayer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
		if (ActiveAudioComponent)
		{
			ActiveAudioComponent->Stop();
			ActiveAudioComponent = nullptr;
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AIHitPlayer::OffSound(const FGameplayTag InputTag, int32 Count)
{
	bool bHasSoundTag = Count > 0;
	if (!bHasSoundTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_AIHitPlayer::OnMontageComplete()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
	ASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIHit);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_AIHitPlayer::FadeOutHitEffect()
{
	if (!PPComp || PPComp->Settings.WeightedBlendables.Array.Num() == 0)
		return;

	FPostProcessSettings NewSettings = PPComp->Settings;
	NewSettings.WeightedBlendables.Array[0].Weight = 0.0f;
	PPComp->Settings = NewSettings;
}