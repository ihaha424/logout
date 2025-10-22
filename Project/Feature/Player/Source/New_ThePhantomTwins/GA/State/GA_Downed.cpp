// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Downed.h"

#include "GM_PhantomTwins.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Objects/HeldItemComponent.h"
#include "TimerManager.h"
#include "Components/PostProcessComponent.h"

UGA_Downed::UGA_Downed()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
	SetAssetTags(DefaultTags);
}

void UGA_Downed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
	GAActorInfo = ActorInfo;
	MyASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed).AddUObject(this, &UGA_Downed::OnDownedTagChanged);

	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	// 손에 뭐 들고있으면 취소시키기
	UHeldItemComponent* HeldItemComp = Character->FindComponentByClass<UHeldItemComponent>();
	HeldItemComp->DestroyHeldItem();
	APS_Player* PS = Cast<APS_Player>(Character->GetPlayerState());
	NULLCHECK_RETURN_LOG(PS, GALog, Warning, );

	APC_Player* PC = Character->GetController<APC_Player>();
	NULLCHECK_RETURN_LOG(PC, GALog, Warning, );
	PC->SetWidget(TEXT("WASD"), true, EMessageTargetType::LocalClient);

	Character->DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	DefaultSocketOffset = Character->GetSpringArm()->SocketOffset;
	if (USpringArmComponent* SpringArm = Character->GetSpringArm())
	{
		SpringArm->SocketOffset = DefaultSocketOffset + FVector(0.f, 0.f, -100.f);
	}

	SetSpeed(DownedSpeed, GAActorInfo);
	// 재시작을 위한 부분
	if (Character->HasAuthority())
	{
		if (AGM_PhantomTwins* GM = GetWorld()->GetAuthGameMode<AGM_PhantomTwins>())
		{
			GM->NotifyPlayerDied(true);
		}
	}

	if (Character->IsLocallyControlled())
	{
		Character->SetFadeVFX(EVignetteType::DownedVignette, 0);
	}
}

void UGA_Downed::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Downed::SetSpeed(float Speed, const FGameplayAbilityActorInfo* ActorInfo)
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

void UGA_Downed::OnDownedTagChanged(const FGameplayTag Tag, int32 TagCount)
{
	bool bHasDownedTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false

	// 스피드 재정의 GE 부여 & 재정의된 스피드 적용
	APlayerCharacter* Character = Cast<APlayerCharacter>(GAActorInfo->AvatarActor.Get());
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, );
	float WalkSpeed = Character->WalkSpeed;
	APS_Player* PS = Cast<APS_Player>(Character->GetPlayerState());
	NULLCHECK_RETURN_LOG(PS, GALog, Warning, );
	APC_Player* PC = Character->GetController<APC_Player>();
	NULLCHECK_RETURN_LOG(PC, GALog, Warning, );

	float FinalSpeed = bHasDownedTag ? DownedSpeed : WalkSpeed;
	SetSpeed(FinalSpeed, GAActorInfo);

	if (!bHasDownedTag)
	{
		PC->SetWidget(TEXT("WASD"), false, EMessageTargetType::LocalClient);
		Character->DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
		Character->GetSpringArm()->SocketOffset = DefaultSocketOffset;
		Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Character->SetFadeVFX(EVignetteType::DownedVignette,1);

		bool bReplicatedEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
	}
}
