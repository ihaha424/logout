
#include "ThrowEMP.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "AI/AIEventReceiver.h"
#include "Log/TPTLog.h"

AThrowEMP::AThrowEMP()
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
}

void AThrowEMP::BeginPlay()
{
	Super::BeginPlay();

    // Hit 이벤트 바인딩
    if (CollisionComponent)
    {
        CollisionComponent->OnComponentHit.AddDynamic(this, &AThrowEMP::OnHit);
    }
}

void AThrowEMP::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 자기 자신이나 소유자와의 충돌은 무시
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        TPT_LOG(GALog, Log, TEXT("AThrowEMP OnHit!!!!!!!!!!!!!!!"));

        ExplodeAndMakeNoise();
    }
}

void AThrowEMP::InvokeGameplayCue()
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

void AThrowEMP::ExplodeAndMakeNoise()
{
    // 투사체 이동 정지
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();
        ProjectileMovementComponent->Deactivate();
    }

    InvokeGameplayCue();

    // 1. 적에게 닿으면 5초간 스턴
    ApplyStunToEnemy();

    // 2. 글리치함정에 닿으면 10초간 비활성화
    DisableGlitchTrap();

    // 일정 시간 후 액터 파괴 (소음이 끝난 후)
    FTimerHandle DestroyTimer;
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
            {
                Destroy();
            }, EnemyStunDuration, false); // 소음이 충분히 지속된 후 파괴
    }
}

void AThrowEMP::ApplyStunToEnemy()
{
    TArray<AActor*> OverlappingEnemys;

    // 적은 인터페이스로 체크

    for (AActor* OverlapEnemy : OverlappingEnemys) // 배열 반복
    {
        if (!OverlapEnemy) continue;

        if (OverlapEnemy->GetClass()->ImplementsInterface(UAIEventReceiver::StaticClass()))
        {
            IAIEventReceiver::Execute_ApplyStun(OverlapEnemy);
            TPT_LOG(GALog, Log, TEXT("Applied stun to enemy: %s"), *OverlapEnemy->GetName());
        }
    }
}

void AThrowEMP::DisableGlitchTrap()
{
    TArray<AActor*> OverlappingGlitchTraps;
    
    // 글리치함정은 tag로 체크

}

