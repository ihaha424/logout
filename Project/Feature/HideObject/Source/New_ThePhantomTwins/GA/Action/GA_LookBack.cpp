// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LookBack.h"
#include "GameFramework/SpringArmComponent.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"

void UGA_LookBack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APlayerCharacter* Player = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
    NULLCHECK_RETURN_LOG(Player, GALog, Error, );
    USpringArmComponent* SpringArm = Player->GetSpringArm();
	NULLCHECK_RETURN_LOG(SpringArm, GALog, Error, );

    OriginalLocation = SpringArm->GetRelativeLocation();

    if (!bIsMoveLeft)
    {
        bIsMoveLeft = true;
        PositionY = OriginalLocation.Y;
        FVector MoveLeft(OriginalLocation.X, OriginalLocation.Y - MoveDistance, OriginalLocation.Z);
        SpringArm->SetRelativeLocation(MoveLeft);
    }
    else
    {
        bIsMoveLeft = false;
        FVector MoveRight(OriginalLocation.X, PositionY, OriginalLocation.Z);
        SpringArm->SetRelativeLocation(MoveRight);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

