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

    // 시작/목표 위치 계산
    const FVector Original = SpringArm->GetRelativeLocation();
    if (!bIsMoveLeft)
    {
        bIsMoveLeft = true;
        PositionY = Original.Y;
        MoveStart = Original;
        MoveTarget = FVector(Original.X, Original.Y - MoveDistance, Original.Z);
    }
    else
    {
        bIsMoveLeft = false;
        MoveStart = Original;
        MoveTarget = FVector(Original.X, PositionY, Original.Z);
    }

    MoveElapsed = 0.f;
    bMoving = true;

    // 타이머 틱 설정: 60FPS 근사
    constexpr float Tick = 1.f / 60.f;
    GetWorld()->GetTimerManager().SetTimer(
        MoveTimerHandle,
        FTimerDelegate::CreateUObject(this, &UGA_LookBack::TickMoveSpringArm, SpringArm),
        Tick, true
    );
}

void UGA_LookBack::TickMoveSpringArm(USpringArmComponent * SpringArm)
{
    if (!bMoving || !SpringArm)
    {
        StopMoveAndEnd();
        return;
    }

    MoveElapsed += GetWorld()->GetDeltaSeconds();
    const float Alpha = FMath::Clamp(MoveElapsed / MoveTotalTime, 0.f, 1.f);

    const FVector NewLoc = FMath::Lerp(MoveStart, MoveTarget, Alpha);
    SpringArm->SetRelativeLocation(NewLoc, false, nullptr, ETeleportType::None);

    if (Alpha >= 1.f)
    {
        StopMoveAndEnd();
    }
}

void UGA_LookBack::StopMoveAndEnd()
{
    bMoving = false;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(MoveTimerHandle);
    }

    // 능력 종료
    // Handle/ActorInfo/ActivationInfo는 멤버에 저장해두거나 EndAbility를 안전하게 호출할 수 있는 시점에 전달
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}