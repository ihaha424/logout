// Fill out your copyright notice in the Description page of Project Settings.

#include "NoiseBomb.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SzComponents/NoiseComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Log/TPTLog.h"

ANoiseBomb::ANoiseBomb()
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

void ANoiseBomb::BeginPlay()
{
    Super::BeginPlay();

    // Hit 이벤트 바인딩 (BeginPlay에서 하는 것이 안전함)
    if (CollisionComponent)
    {
        CollisionComponent->OnComponentHit.AddDynamic(this, &ANoiseBomb::OnHit);
    }
}

void ANoiseBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 자기 자신이나 소유자와의 충돌은 무시
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        ExplodeAndMakeNoise();
    }
}

void ANoiseBomb::ExplodeAndMakeNoise()
{
    // 투사체 이동 정지
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
        ProjectileMovementComponent->Deactivate();
    }

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
