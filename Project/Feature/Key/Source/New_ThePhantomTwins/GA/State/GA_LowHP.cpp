// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LowHP.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "TimerManager.h"

UGA_LowHP::UGA_LowHP()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; // 오히려 이거로 하니까 클라에는 하나가 더 붙음. // 서버는 이제 딱 하나만 붙게됨.
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP);
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

		PPComp = Character->PostProcessComponent;
		NULLCHECK_RETURN_LOG(PPComp, GALog, Error, )

		CurrentWeight = PPComp->Settings.WeightedBlendables.Array[0].Weight;
		FinalWeight = 1.0f;
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UGA_LowHP::UpdateFade, 0.016f, true); // 약 60FPS
	}
}

void UGA_LowHP::OnLowHPTagChanged(const FGameplayTag Tag, int32 TagCount)
{
	bool bHasLowHPTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false

	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	if (!bHasLowHPTag)
	{
		FinalWeight = 0.0f;
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &UGA_LowHP::UpdateFade, 0.016f, true);
	}
}
void UGA_LowHP::UpdateFade()
{
	if (!PPComp || PPComp->Settings.WeightedBlendables.Array.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
		return;
	}

	CurrentWeight = FMath::FInterpTo(CurrentWeight, FinalWeight, 0.016f, InterpSpeed);
	PPComp->Settings.WeightedBlendables.Array[0].Weight = CurrentWeight;

	// 종료 조건: 거의 같아지면 정확히 맞춰주고 타이머 해제
	if (FMath::IsNearlyEqual(CurrentWeight, FinalWeight, 0.01f))
	{
		PPComp->Settings.WeightedBlendables.Array[0].Weight = FinalWeight;
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);

		// Weight 0이면 어빌리티 종료
		if (FMath::IsNearlyZero(FinalWeight))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

