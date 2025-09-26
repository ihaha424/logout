// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowNoiseBomb.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SzComponents/NoiseComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Log/TPTLog.h"

AThrowNoiseBomb::AThrowNoiseBomb()
{
    PrimaryActorTick.bCanEverTick = false;

    // 충돌 컴포넌트 (루트)
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(10.0f);
    //CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true); // Hit 이벤트 생성

    // 루트로 설정
    RootComponent = CollisionComponent;

    // 메시 컴포넌트 (충돌은 CollisionComponent가 담당)
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetRelativeLocation(FVector::ZeroVector);

    // 투사체 이동 컴포넌트
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->InitialSpeed = 1200.0f;
    ProjectileMovementComponent->MaxSpeed = 2000.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 1.0f;

    // 소음 컴포넌트
    NoiseComponent = CreateDefaultSubobject<UNoiseComponent>(TEXT("NoiseComponent"));
}

void AThrowNoiseBomb::BeginPlay()
{
    Super::BeginPlay();

    // Hit 이벤트 바인딩
    if (CollisionComponent)
    {
        CollisionComponent->OnComponentHit.AddDynamic(this, &AThrowNoiseBomb::OnHit);
    }
}

void AThrowNoiseBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 자기 자신이나 소유자와의 충돌은 무시
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        // 투사체 이동 정지
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->StopMovementImmediately();
            ProjectileMovementComponent->Deactivate();
        }

        // 화이트리스트 기반 붙기 판정[41][46]
        bool bCanStickToSurface = CanStickToActor(OtherActor);

        if (bCanStickToSurface)
        {
            // 특정 태그가 있는 장애물에만 붙기
            StickToSurface(OtherActor, Hit);
            UE_LOG(LogTemp, Warning, TEXT("NoiseBomb stuck to surface: %s"), *OtherActor->GetName());
        }
        else
        {
            // 기본 동작: 바닥으로 떨어뜨리기 (아이템, 적 등 모든 기타 오브젝트)
            FallToGround();
            UE_LOG(LogTemp, Warning, TEXT("NoiseBomb falling to ground after hitting: %s"), *OtherActor->GetName());
        }

        // 폭발 및 소음 시작
        ExplodeAndMakeNoise();
    }
}

void AThrowNoiseBomb::InvokeGameplayCue()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->GetPawn())
    {
        AActor* TargetActor = PC->GetPawn();
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

        if (TargetASC)
        {
            FGameplayCueParameters Param;
            Param.SourceObject = this;
            Param.Instigator = TargetActor;
            Param.Location = GetActorLocation();
            TargetASC->ExecuteGameplayCue(GameplayCueTag, Param);
        }
    }
}

void AThrowNoiseBomb::ExplodeAndMakeNoise()
{
    InvokeGameplayCue();

    // 소음 시작
    if (NoiseComponent)
    {
        NoiseComponent->StartNoise();
    }

    // 일정 시간 후 액터 파괴 (소음이 끝난 후)
    FTimerHandle DestroyTimer;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
            {
                NoiseComponent->StopNoise();
                Destroy();
            }, NoiseDuration, false); // 소음이 충분히 지속된 후 파괴
    }
}

bool AThrowNoiseBomb::CanStickToActor(AActor* Actor)
{
    if (!Actor)
        return false;

    // 붙을 수 있는 태그들만 화이트리스트로 지정
    TArray<FName> StickableTags = {
        TEXT("Wall"),
        TEXT("Ground"),
        TEXT("AttachableObject"),
        TEXT("Ceiling")
    };

    // 붙을 수 있는 태그가 있는지 확인
    for (const FName& Tag : StickableTags)
    {
        if (Actor->ActorHasTag(Tag))
        {
            UE_LOG(LogTemp, Warning, TEXT("NoiseBomb can stick to actor: %s (Tag: %s)"),
                *Actor->GetName(), *Tag.ToString());
            return true;
        }
    }

    // 태그가 없거나 화이트리스트에 없는 경우 붙을 수 없음 (기본 동작)
    UE_LOG(LogTemp, Warning, TEXT("NoiseBomb cannot stick to actor: %s (No matching stick tags)"),
        *Actor->GetName());
    return false;
}

void AThrowNoiseBomb::StickToSurface(AActor* SurfaceActor, const FHitResult& HitResult)
{
    if (!SurfaceActor)
        return;

    // 물리 시뮬레이션 완전 정지
    if (CollisionComponent)
    {
        CollisionComponent->SetSimulatePhysics(false);
        CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
        CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 충돌 지점에 정확히 위치시키기
    FVector HitLocation = HitResult.Location;
    FVector HitNormal = HitResult.Normal;

    // 표면에서 약간 떨어진 위치로 조정 (겹침 방지)
    FVector AdjustedLocation = HitLocation + (HitNormal * 2.0f);
    SetActorLocation(AdjustedLocation);

    // 표면 법선을 향하도록 회전 (선택사항)
    FRotator SurfaceRotation = FRotationMatrix::MakeFromZ(HitNormal).Rotator();
    SetActorRotation(SurfaceRotation);

    // 만약 해당 액터가 움직이는 오브젝트라면 붙이기 (AttachToActor 사용)
    UPrimitiveComponent* HitComponent = Cast<UPrimitiveComponent>(HitResult.GetComponent());
    if (HitComponent && HitComponent->IsSimulatingPhysics())
    {
        // 움직이는 오브젝트에 물리적으로 붙이기
        AttachToActor(SurfaceActor, FAttachmentTransformRules::KeepWorldTransform);
        UE_LOG(LogTemp, Warning, TEXT("NoiseBomb attached to moving object: %s"), *SurfaceActor->GetName());
    }
    else
    {
        // 정적 오브젝트는 위치만 고정
        UE_LOG(LogTemp, Warning, TEXT("NoiseBomb stuck to static surface: %s"), *SurfaceActor->GetName());
    }
}

void AThrowNoiseBomb::FallToGround()
{
    // 물리 시뮬레이션을 다시 활성화하여 바닥으로 떨어지게 함
    if (CollisionComponent)
    {
        CollisionComponent->SetSimulatePhysics(true);
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        // 아래쪽으로만 떨어지도록 속도 조정
        FVector DownwardVelocity = FVector(0.0f, 0.0f, -800.0f);
        CollisionComponent->SetPhysicsLinearVelocity(DownwardVelocity);

        // 수평 속도는 제거
        FVector CurrentVelocity = CollisionComponent->GetPhysicsLinearVelocity();
        CurrentVelocity.X = 0.0f;
        CurrentVelocity.Y = 0.0f;
        CollisionComponent->SetPhysicsLinearVelocity(CurrentVelocity);

        UE_LOG(LogTemp, Warning, TEXT("NoiseBomb falling straight down"));
    }

    // TWeakObjectPtr을 사용한 안전한 람다 캡처
    TWeakObjectPtr<AThrowNoiseBomb> WeakThis = this;

    GetWorld()->GetTimerManager().SetTimer(GroundCheckTimer,
        FTimerDelegate::CreateWeakLambda(this, [WeakThis]()
            {
                // 객체가 아직 유효한지 확인
                if (WeakThis.IsValid())
                {
                    if (WeakThis->CollisionComponent)
                    {
                        FVector Velocity = WeakThis->CollisionComponent->GetPhysicsLinearVelocity();
                        // 속도가 거의 0에 가까우면 바닥에 안착했다고 판단
                        if (FMath::Abs(Velocity.Z) < 50.0f && Velocity.Size() < 100.0f)
                        {
                            WeakThis->CollisionComponent->SetSimulatePhysics(false);
                            WeakThis->CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
                            UE_LOG(LogTemp, Warning, TEXT("NoiseBomb settled on ground"));

                            // 타이머 정리
                            if (WeakThis->GetWorld())
                            {
                                WeakThis->GetWorld()->GetTimerManager().ClearTimer(WeakThis->GroundCheckTimer);
                            }
                        }
                    }
                }
            }), 0.1f, true);
}
