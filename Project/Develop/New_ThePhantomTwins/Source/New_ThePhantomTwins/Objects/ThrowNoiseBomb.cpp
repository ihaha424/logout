// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowNoiseBomb.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SzComponents/NoiseComponent.h"
#include "TimerManager.h"
#include "Log/TPTLog.h"

AThrowNoiseBomb::AThrowNoiseBomb()
{
    PrimaryActorTick.bCanEverTick = false;

    // 충돌 컴포넌트 (루트)
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(10.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true); // Hit 이벤트 생성

    // 루트로 설정
    RootComponent = CollisionComponent;

    // 메시 컴포넌트 (충돌은 CollisionComponent가 담당)
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    //MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
        //TPT_LOG(GALog, Log, TEXT("AThrowNoiseBomb OnHit!!!!!!!!!!!!!!!"));

        // ProjectileMovementComponent 비활성화
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->StopMovementImmediately();
            ProjectileMovementComponent->Deactivate();
        }

        // 물리 시뮬레이션 활성화, 중력 적용
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetEnableGravity(true);

		CollisionComponent->SetSimulatePhysics(true);
		CollisionComponent->SetEnableGravity(true);


        // 바닥 판정 (예: 태그 "Ground")
        if (OtherActor->ActorHasTag(FName("Ground")))
        {
            //TPT_LOG(ObjectLog, Log, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));

            MeshComponent->SetSimulatePhysics(false);
            MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
            MeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

            CollisionComponent->SetSimulatePhysics(false);
            CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
            CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

            ExplodeAndMakeNoise();
        }
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
    //TPT_LOG(ObjectLog, Log, TEXT("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"));

    InvokeGameplayCue();

    // 소음 시작
    if (NoiseComponent)
    {
        NoiseComponent->StartNoise();
    }

    // 일정 시간 후 액터 파괴 (소음이 끝난 후)
    FTimerHandle DestroyTimer;
    if (UWorld* W = GetWorld())
    {
        FTimerDelegate Del;
        Del.BindWeakLambda(this, [this]()
            {
                if (IsValid(NoiseComponent)) 
                { 
                    NoiseComponent->StopNoise(); 
                }
                DestroyNoiseBomb();
                Destroy(); // 여기서 파괴해도 람다 종료로 안전
            });
        W->GetTimerManager().SetTimer(DestroyTimer, Del, NoiseDuration, false);
    }
}


void AThrowNoiseBomb::DestroyNoiseBomb()
{
    // 이 액터 내 모든 UStaticMeshComponent를 찾아서 처리
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    GetComponents<UStaticMeshComponent>(StaticMeshComponents);

    for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetHiddenInGame(true);
            MeshComp->SetVisibility(false);
            MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }

    // 이 액터 내 모든 UNiagaraComponent를 찾아서 처리
    TArray<UNiagaraComponent*> NiagaraComponents;
    GetComponents<UNiagaraComponent>(NiagaraComponents);

    for (UNiagaraComponent* NiagaraComp : NiagaraComponents)
    {
        if (NiagaraComp)
        {
            NiagaraComp->Activate(false);
            NiagaraComp->SetHiddenInGame(true);
            NiagaraComp->Deactivate();
            NiagaraComp->SetVisibility(false);
            NiagaraComp->SetComponentTickEnabled(false);
        }
    }

    SetActorEnableCollision(false);	// 더이상 이벤트가 일어나지 않도록 false
    SetLifeSpan(6.0f);				// 2초뒤에 자동으로 사라지도록.
}
