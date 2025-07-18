// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_Run::UGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CancelTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo = ActorInfo;
	GAActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffect, MyASC);

	MyASC->CancelAbilities(&CancelTags);
	// 스프린트로 인한 달리기 속도 변경을 태그바인딩을 통해 실행.
	MyASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_Sprint).AddUObject(this, &UGA_Run::OnSprintTagChanged);
	TPT_LOG(GALog, Error, TEXT("6"));
	// 스태미너 감소 GE 부여
	FGameplayEffectSpecHandle StaminaDrainEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaDrainEffect, GetAbilityLevel());
	if (StaminaDrainEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaDrainEffectSpecHandle);
	}

	SetSpeed(OutPutRunSpeed, GAActorInfo);

	// 스태미너가 0이되면 강제 중지
	GetWorld()->GetTimerManager().SetTimer(StaminaCheckHandle, [this]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
			NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

			const UPlayerAttributeSet* Attr = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();
			if (Attr->GetStamina() <= 0.f)
			{
				// 스태미나 0이면 종료
				CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			}
		}, 0.1f, true); // 0.1초마다 체크


    // 몽타주 재생
	//if (RunningMontage)
	//{
	//    PlayMontage(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), RunningMontage);
	//}
}

void UGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	GAActorInfo = ActorInfo;
	// 스태미너 감소 GE 제거
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaDrainEffect, MyASC);

	// 스태미너 재생 GE 부여
	FGameplayEffectSpecHandle StaminaRegenEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaRegenEffect, GetAbilityLevel());
	if (StaminaRegenEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaRegenEffectSpecHandle);
	}

	// 스피드 재정의 GE 부여 & 재정의된 스피드 적용
	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	float WalkSpeed = Character->WalkSpeed;
	SetSpeed(WalkSpeed, GAActorInfo);

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGA_Run::SetSpeed(float Speed, const FGameplayAbilityActorInfo* ActorInfo)
{
	// 스피드 재정의 GE 부여
	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, Speed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	// 재정의된 스피드 적용
	FTimerHandle TimerHandle;
	ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
		TimerHandle, [this, ActorInfo]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
			NULLCHECK_RETURN_LOG(Character, GALog, Warning, )

				const UPlayerAttributeSet* Attribute = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>();

			float Speed = Attribute->GetFinalSpeed();

			Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}, 0.05f, false); // 0.05초 정도 후에 반영

}

void UGA_Run::OnSprintTagChanged(const FGameplayTag Tag, int32 TotalCount)
{
	bHasSprintTag = TotalCount > 0; // 태그가 붙었으면 true, 없으면 false
	TPT_LOG(GALog, Error, TEXT("7    %d"), bHasSprintTag);
	UpdateMoveSpeed();
}

void UGA_Run::UpdateMoveSpeed()
{
	float FinalSpeed = bHasSprintTag ? BaseRunSpeed * SprintMultiplier : BaseRunSpeed;
	OutPutRunSpeed = FinalSpeed;
	SetSpeed(OutPutRunSpeed, GAActorInfo);
}