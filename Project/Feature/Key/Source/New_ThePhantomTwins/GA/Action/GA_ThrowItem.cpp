// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ThrowItem.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Objects/ThrowEMP.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Log/TPTLog.h"

UGA_ThrowItem::UGA_ThrowItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_ThrowItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthority(&ActivationInfo))
    {
        EItemType ItemType = EItemType::None;

        // TriggerEventData에서 ItemType 추출
        if (TriggerEventData)
        {
            ItemType = static_cast<EItemType>((int32)TriggerEventData->EventMagnitude);
        }

        SpawnThrowableItem(ItemType);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_ThrowItem::SpawnThrowableItem(EItemType ItemType)
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

    // 1) 스폰 시작 위치: RightHandSocket (있으면)
    FVector SpawnLocation = GetRightHandSocketLocation();
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

    UWorld* World = GetWorld();
    if (!World) return;


    // 5) Launch Velocity 계산 (포물선 궤적)
    FVector LaunchVelocity;
    bool bHaveVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        World,
        LaunchVelocity,
        SpawnLocation,
        TargetLocation,
        0.0f,     // Override gravity Z (0 -> use world gravity)
        Arc      // Arc parameter
    );

    // 6) 액터 스폰
    switch (ItemType)
    {
    case EItemType::NoiseBomb:
    {
        AThrowNoiseBomb* ThrowActor = World->SpawnActor<AThrowNoiseBomb>(NoiseBombClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (!ThrowActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn ThrowActor."));
            return;
        }

        TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem :: NoiseBomb 생성"));

        if (bHaveVelocity && ThrowActor->ProjectileMovementComponent)
        {
            // ProjectileMovementComponent에 속도 적용
            ThrowActor->ProjectileMovementComponent->Velocity = LaunchVelocity;
            ThrowActor->ProjectileMovementComponent->SetActive(true);
            ThrowActor->ProjectileMovementComponent->Activate(true);
            ThrowActor->ProjectileMovementComponent->UpdateComponentVelocity();
        }
        else if (ThrowActor->ProjectileMovementComponent)
        {
            // 실패하면 그냥 앞으로 일정 속도 부여
            FVector Fwd = SpawnRotation.Vector();
            ThrowActor->ProjectileMovementComponent->Velocity = Fwd * ThrowActor->ProjectileMovementComponent->InitialSpeed;
            ThrowActor->ProjectileMovementComponent->SetActive(true);
            ThrowActor->ProjectileMovementComponent->Activate(true);
            ThrowActor->ProjectileMovementComponent->UpdateComponentVelocity();
        }

        break;
    }
    case EItemType::EMP:
    {
        AThrowEMP* ThrowActor = World->SpawnActor<AThrowEMP>(EMPClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (!ThrowActor)
        {
            TPT_LOG(GALog, Warning, TEXT("Failed to spawn ThrowActor."));
            return;
        }

        TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem :: ThrowEMP 생성"));

        if (bHaveVelocity && ThrowActor->ProjectileMovementComponent)
        {
            // ProjectileMovementComponent에 속도 적용
            ThrowActor->ProjectileMovementComponent->Velocity = LaunchVelocity;
            ThrowActor->ProjectileMovementComponent->SetActive(true);
            ThrowActor->ProjectileMovementComponent->Activate(true);
            ThrowActor->ProjectileMovementComponent->UpdateComponentVelocity();
        }
        else if (ThrowActor->ProjectileMovementComponent)
        {
            // 실패하면 그냥 앞으로 일정 속도 부여
            FVector Fwd = SpawnRotation.Vector();
            ThrowActor->ProjectileMovementComponent->Velocity = Fwd * ThrowActor->ProjectileMovementComponent->InitialSpeed;
            ThrowActor->ProjectileMovementComponent->SetActive(true);
            ThrowActor->ProjectileMovementComponent->Activate(true);
            ThrowActor->ProjectileMovementComponent->UpdateComponentVelocity();
        }

        break;
    }

    default:
        break;
    }
}

FVector UGA_ThrowItem::CalculateTargetLocation(const FVector& StartLocation)
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

FVector UGA_ThrowItem::GetRightHandSocketLocation() const
{
    AActor* OwnerAvatar = GetAvatarActorFromActorInfo();
    if (!OwnerAvatar) return FVector::ZeroVector;

    // 만약 ACharacter라면 SkeletalMesh의 소켓 위치를 사용
    const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerAvatar);

    if (PlayerChar)
    {
        if (const USkeletalMeshComponent* MeshComp = PlayerChar->GetMesh())
        {
            if (MeshComp->DoesSocketExist(TEXT("RightHandSocket")))
            {
                return MeshComp->GetSocketLocation(TEXT("RightHandSocket"));
            }
        }
    }

    // 소켓이 없으면 액터 위치 반환 (caller에서 예외 처리)
    return FVector::ZeroVector;
}

FRotator UGA_ThrowItem::GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const
{
    return UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
}
