// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Confused2nd.h"
#include "Log/TPTLog.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/TPTGameplayTags.h"

UGA_Confused2nd::UGA_Confused2nd()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd);
	SetAssetTags(DefaultTags);
}

void UGA_Confused2nd::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, );
	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd).AddUObject(this, &UGA_Confused2nd::OffSound);

	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
		if (USoundBase* Sound = SoundCue1st) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent1st = UGameplayStatics::SpawnSoundAttached(Sound, ActorInfo->AvatarActor->GetRootComponent());
		}
		if (USoundBase* Sound = SoundCue2nd) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
		{
			ActiveAudioComponent2nd = UGameplayStatics::SpawnSoundAttached(Sound, ActorInfo->AvatarActor->GetRootComponent());
		}
	}
}
void UGA_Confused2nd::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
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
void UGA_Confused2nd::OffSound(const FGameplayTag InputTag, int32 Count)
{
	bool bHasSoundTag = Count > 0;
	if (!bHasSoundTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}