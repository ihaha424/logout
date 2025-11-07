// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Confused3rd.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_Confused3rd::UGA_Confused3rd()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd);
	SetAssetTags(DefaultTags);
}

void UGA_Confused3rd::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, );
	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd).AddUObject(this, &UGA_Confused3rd::OffSound);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (Character && Character->IsLocallyControlled())
	{
		if (USoundBase* Sound = SoundCue1st) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent1st = UGameplayStatics::SpawnSoundAttached(Sound, Character->GetRootComponent());
		}
		if (USoundBase* Sound = SoundCue2nd) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent2nd = UGameplayStatics::SpawnSoundAttached(Sound, Character->GetRootComponent());
		}
		Character->SetFadeVFX(EVignetteType::Confused3rdVignette, 0);
	}
}

void UGA_Confused3rd::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetActorInfo().AvatarActor);
	NULLCHECK_RETURN_LOG(Character, GALog, Error, );

	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
		Character->SetFadeVFX(EVignetteType::Confused3rdVignette, 1);

		if (ActiveAudioComponent1st)
		{
			ActiveAudioComponent1st->Stop();
			ActiveAudioComponent1st = nullptr;
		}
		if (ActiveAudioComponent2nd)
		{
			ActiveAudioComponent2nd->Stop();
			ActiveAudioComponent2nd = nullptr;
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Confused3rd::OffSound(const FGameplayTag InputTag, int32 Count)
{
	bool bHasSoundTag = Count > 0;
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetActorInfo().AvatarActor);
	NULLCHECK_RETURN_LOG(Character, GALog, Error, );

	if (!bHasSoundTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
