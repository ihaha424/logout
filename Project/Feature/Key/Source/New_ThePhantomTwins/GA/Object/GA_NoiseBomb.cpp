// Fill out your copyright notice in the Description page of Project Settings.

#include "GA_NoiseBomb.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Log/TPTLog.h"

UGA_NoiseBomb::UGA_NoiseBomb()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_NoiseBomb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthority(&ActivationInfo))
    {
        SpawnNoiseBomb();
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_NoiseBomb::SpawnNoiseBomb()
{
    if (!NoiseBombClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoiseBombClass is not set!"));
        return;
    }

    AActor* OwnerActor = GetOwningActorFromActorInfo();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("UGA_NoiseBomb: No owning actor."));
        return;
    }

    // 1) 스폰 시작 위치: LeftHandSocket (있으면)
    FVector SpawnLocation = GetLeftHandSocketLocation();
    if (SpawnLocation.IsZero())
    {
        // 실패 시 액터 위치를 사용
        SpawnLocation = OwnerActor->GetActorLocation();
    }

    // 2) 목표 위치 계산 (시야/컨트롤러 방향 우선, 없으면 Actor Forward)
    FVector TargetLocation = CalculateTargetLocation(SpawnLocation);

    // 3) 스폰 회전 (LookAt)
    FRotator SpawnRotation = GetThrowRotation(SpawnLocation, TargetLocation);

    // 4) 스폰 파라미터
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerActor;
    SpawnParams.Instigator = Cast<APawn>(OwnerActor);
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 5) 액터 스폰
    UWorld* World = GetWorld();
    if (!World) return;

    AThrowNoiseBomb* NoiseBomb = World->SpawnActor<AThrowNoiseBomb>(NoiseBombClass, SpawnLocation, SpawnRotation, SpawnParams);
    if (!NoiseBomb)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn NoiseBomb actor."));
        return;
    }
    else
    {
        TPT_LOG(GALog, Log, TEXT("AThrowNoiseBomb 생성"));
    }

    // 6) Launch Velocity 계산 (포물선 궤적)
    FVector LaunchVelocity;
    bool bHaveVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        World,
        LaunchVelocity,
        SpawnLocation,
        TargetLocation,
        0.0f,     // Override gravity Z (0 -> use world gravity)
        Arc      // Arc parameter
    );

    if (bHaveVelocity && NoiseBomb->ProjectileMovementComponent)
    {
        // ProjectileMovementComponent에 속도 적용
        NoiseBomb->ProjectileMovementComponent->Velocity = LaunchVelocity;
        NoiseBomb->ProjectileMovementComponent->SetActive(true);
        NoiseBomb->ProjectileMovementComponent->Activate(true);
        NoiseBomb->ProjectileMovementComponent->UpdateComponentVelocity();
    }
    else if (NoiseBomb->ProjectileMovementComponent)
    {
        // 실패하면 그냥 앞으로 일정 속도 부여
        FVector Fwd = SpawnRotation.Vector();
        NoiseBomb->ProjectileMovementComponent->Velocity = Fwd * NoiseBomb->ProjectileMovementComponent->InitialSpeed;
        NoiseBomb->ProjectileMovementComponent->SetActive(true);
        NoiseBomb->ProjectileMovementComponent->Activate(true);
        NoiseBomb->ProjectileMovementComponent->UpdateComponentVelocity();
    }
}

FVector UGA_NoiseBomb::CalculateTargetLocation(const FVector& StartLocation)
{
    AActor* OwnerActor = GetOwningActorFromActorInfo();
    if (!OwnerActor)
        return StartLocation + FVector::ForwardVector * ThrowDistance;

    // 우선적으로 컨트롤러의 뷰 방향(카메라)을 사용
    FVector ViewDir = FVector::ZeroVector;
    APawn* Pawn = Cast<APawn>(OwnerActor);
    if (Pawn && Pawn->GetController())
    {
        FRotator ControlRot = Pawn->GetController()->GetControlRotation();
        ViewDir = ControlRot.Vector();
    }

    if (ViewDir.IsNearlyZero())
    {
        // 없으면 액터 전방 사용
        ViewDir = OwnerActor->GetActorForwardVector();
    }

    return StartLocation + ViewDir.GetSafeNormal() * ThrowDistance;
}

FVector UGA_NoiseBomb::GetLeftHandSocketLocation() const
{
    AActor* OwnerAvatar = GetAvatarActorFromActorInfo();
    if (!OwnerAvatar) return FVector::ZeroVector;

    // 만약 ACharacter라면 SkeletalMesh의 소켓 위치를 사용
    const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerAvatar);

    if (PlayerChar)
    {
        if (const USkeletalMeshComponent* MeshComp = PlayerChar->GetMesh())
        {
            if (MeshComp->DoesSocketExist(TEXT("LeftHandSocket")))
            {
                return MeshComp->GetSocketLocation(TEXT("LeftHandSocket"));
            }
        }
    }

    // 소켓이 없으면 액터 위치 반환 (caller에서 예외 처리)
    return FVector::ZeroVector;
}

FRotator UGA_NoiseBomb::GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const
{
    return UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
}
