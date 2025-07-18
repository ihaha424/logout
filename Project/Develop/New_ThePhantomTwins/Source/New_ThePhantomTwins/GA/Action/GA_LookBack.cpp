// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_LookBack.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"

void UGA_LookBack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APlayerCharacter* Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
    NULLCHECK_RETURN_LOG(Player, GALog, Error, );

	Player->SpringArm->TargetArmLength = -Player->SpringArm->TargetArmLength;
	OriginalRotation = Player->Camera->GetRelativeRotation();

    FRotator FlipRot = OriginalRotation;
    FlipRot.Yaw += 180.f;
    Player->Camera->SetRelativeRotation(FlipRot);
}

void UGA_LookBack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    APlayerCharacter* Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
    NULLCHECK_RETURN_LOG(Player, GALog, Error, );

    Player->SpringArm->TargetArmLength = -Player->SpringArm->TargetArmLength;
	Player->Camera->SetRelativeRotation(OriginalRotation); // 저장된 각도로 복귀
    
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
