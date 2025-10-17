// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LowHP.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Components/PostProcessComponent.h"
#include "Tags/TPTGameplayTags.h"

UGA_LowHP::UGA_LowHP()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
	SetAssetTags(DefaultTags);
}

void UGA_LowHP::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );
	
	if (Character->IsLocallyControlled())
	{
		GAActorInfo = ActorInfo;
		UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
		MyASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP).AddUObject(this, &UGA_LowHP::OnLowHPTagChanged);
	}
}

void UGA_LowHP::OnLowHPTagChanged(const FGameplayTag Tag, int32 TagCount)
{
	bool bHasLowHPTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false

	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
}