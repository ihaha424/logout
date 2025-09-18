// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"

UGA_Run::UGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	CancelTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch);
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run);
}

void UGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo = ActorInfo;
	GAActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffect, MyASC);
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

	MyASC->CancelAbilities(&CancelTags);
	// 스프린트로 인한 달리기 속도 변경을 태그바인딩을 통해 실행.
	MyASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_Sprinting).AddUObject(this, &UGA_Run::OnSprintTagChanged);

	SetSpeed(OutPutRunSpeed, GAActorInfo);
	bool bIsMoving = Character->GetCharacterMovement()->Velocity.Size() > 3.0f;

	if (!bIsMoving)
	{
		bIsStaminaRegen = true;
		StaminaRegen();
	}
	else
	{
		bIsStaminaRegen = false;
		StaminaDrain();
	}

	// 캐릭터가 실제로 이동을 할 때에만 스태미나가 닳도록 체크하기.
	GetWorld()->GetTimerManager().SetTimer(MovementCheckHandle, [this]()
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
			NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
			bool bIsMoving = Character->GetCharacterMovement()->Velocity.Size() > 3.0f;
			// 움직임이 없게된다면 스태미나 다시 증가.
			if (!bIsMoving && !bIsStaminaRegen)
			{
				bIsStaminaRegen = true;
				StaminaRegen();
			}
			else if (bIsMoving && bIsStaminaRegen)
			{
				bIsStaminaRegen = false;
				StaminaDrain();
			}
		}, 0.1f, true);

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
}

void UGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (!ActorInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGA_Run::CancelAbility] ActorInfo is NULL. Ability cleanup skipped."));
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
		return;
	}

	if (!ActorInfo->AbilitySystemComponent.Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGA_Run::CancelAbility] AbilitySystemComponent is NULL. Owner may be destroyed. Skipping cleanup."));
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
		return;
	}
	GAActorInfo = ActorInfo;

	GetWorld()->GetTimerManager().ClearTimer(MovementCheckHandle);
	GetWorld()->GetTimerManager().ClearTimer(StaminaCheckHandle);

	StaminaRegen();

	// 다운드 되면 SetSpeed를 하지않아도 됨.
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	if (MyASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed)|| MyASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd))
	{
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
		return;
	}

	// 스피드 재정의 GE 부여 & 재정의된 스피드 적용
	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	float WalkSpeed = Character->WalkSpeed;
	SetSpeed(WalkSpeed, GAActorInfo);

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Run::StaminaDrain()
{
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffect, MyASC);
	// 스태미너 감소 GE 부여
	FGameplayEffectSpecHandle StaminaDrainEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaDrainEffect, GetAbilityLevel());
	if (StaminaDrainEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaDrainEffectSpecHandle);
	}
}

void UGA_Run::StaminaRegen()
{
	// 스태미너 감소 GE 제거
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(StaminaDrainEffect, MyASC);

	// 스태미너 재생 GE 부여
	FGameplayEffectSpecHandle StaminaRegenEffectSpecHandle = MakeOutgoingGameplayEffectSpec(StaminaRegenEffect, 1.0);
	if (StaminaRegenEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, StaminaRegenEffectSpecHandle);
	}
}


void UGA_Run::SetSpeed(float Speed, const FGameplayAbilityActorInfo* ActorInfo)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;

	// 스피드 재정의 GE 부여
	FGameplayEffectSpecHandle SetSpeedEffectSpecHandle = MakeOutgoingGameplayEffectSpec(SetSpeedEffect, 1.0f);
	if (SetSpeedEffectSpecHandle.IsValid())
	{
		FGameplayTag SpeedOverrideTag = FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MoveSpeed;
		SetSpeedEffectSpecHandle.Data->SetSetByCallerMagnitude(SpeedOverrideTag, Speed);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SetSpeedEffectSpecHandle);
	}
}

void UGA_Run::OnSprintTagChanged(const FGameplayTag Tag, int32 TagCount)
{
	bHasSprintTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false

	float FinalSpeed = bHasSprintTag ? BaseRunSpeed * SprintMultiplier : BaseRunSpeed;
	OutPutRunSpeed = FinalSpeed;
	SetSpeed(OutPutRunSpeed, GAActorInfo);
}
