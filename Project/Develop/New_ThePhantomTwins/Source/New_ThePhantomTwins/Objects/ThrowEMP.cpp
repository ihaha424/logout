
#include "ThrowEMP.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "player/PlayerCharacter.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/AIEventReceiver.h"
#include "Objects/GlitchTrap.h"
#include "Log/TPTLog.h"

AThrowEMP::AThrowEMP()
{
	PrimaryActorTick.bCanEverTick = false;

    SetReplicates(true);  // 복제 활성화
    SetReplicateMovement(true);  // 이동 복제 활성화

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

    MeshComponent->SetIsReplicated(true);

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

void AThrowEMP::EndPlay(EEndPlayReason::Type Reason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
    }

	Super::EndPlay(Reason);
}

void AThrowEMP::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 자기 자신이나 소유자와의 충돌은 무시
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        //TPT_LOG(GALog, Log, TEXT("AThrowEMP OnHit!!!!!!!!!!!!!!!"));

        // ProjectileMovementComponent 비활성화
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->StopMovementImmediately();
            ProjectileMovementComponent->Deactivate();
        }

        // 메시의 물리 시뮬레이션 활성화, 중력 적용
		if (MeshComponent)
		{
			MeshComponent->SetSimulatePhysics(true);
			MeshComponent->SetEnableGravity(true);

            // 바닥 판정 (예: 태그 "Ground")
            if (OtherActor->ActorHasTag(FName("Ground")))
            {
                MeshComponent->SetSimulatePhysics(false);
                MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
                MeshComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            }
		}

        if (CollisionComponent)
        {
            CollisionComponent->SetSimulatePhysics(true);
            CollisionComponent->SetEnableGravity(true);


            // 바닥 판정 (예: 태그 "Ground")
            if (OtherActor->ActorHasTag(FName("Ground")))
            {
                CollisionComponent->SetSimulatePhysics(false);
                CollisionComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
                CollisionComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

                ExplodeAndMakeNoise();
            }
        }
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
    InvokeGameplayCue();

    // 1. 적에게 닿으면 5초간 스턴
    ApplyStunToEnemy();

    // 2. 글리치함정에 닿으면 10초간 비활성화
    DisableGlitchTrap();

    // 일정 시간 후 액터 파괴 (소음이 끝난 후)
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
    //TPT_LOG(GALog, Log, TEXT("AThrowEMP::DisableGlitchTrap()"));

    TArray<AActor*> OverlappingEnemys;

    // 검사할 오브젝트 타입 설정 (Pawn 타입 => 적/캐릭터를 대상으로)
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    // 무시할 액터
    TArray<AActor*> ActorsToIgnore;

    TArray<AActor*> AllPlayerCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayerCharacters);

    // 찾은 모든 플레이어 캐릭터를 무시 리스트에 추가
    for (AActor* PlayerCharacter : AllPlayerCharacters)
    {
        if (PlayerCharacter)
        {
            ActorsToIgnore.Add(PlayerCharacter);
            //TPT_LOG(GALog, Log, TEXT("Added PlayerCharacter to ignore list: %s"), *PlayerCharacter->GetName());
        }
    }

// 디버깅
//#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
//    DrawDebugSphere(
//        GetWorld(),                        // 월드 컨텍스트
//        GetActorLocation(),                // 구의 중심 위치
//        EnemyStunRadius,                   // 구의 반경
//        12,                                // 구체의 세분화 정도 (원형면의 세그먼트 수, 적절하게 조절)
//        FColor::Red,                      // 색상
//        false,                            // 지속 시간 (false면 한 프레임만)
//        2.0f                              // 지속 시간 (초)
//    );
//#endif

    // AAIBaseCharacter 클래스 필터를 사용해서 적 캐릭터만 검색
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),                    // WorldContextObject
        GetActorLocation(),      // SpherePos - 구의 중심 위치
        EnemyStunRadius,         // SphereRadius - 구의 반경
        ObjectTypes,             // ObjectTypes - Pawn 타입만 검색
        AAIBaseCharacter::StaticClass(), // ActorClassFilter - AAIBaseCharacter 클래스만 필터링
        ActorsToIgnore,          // ActorsToIgnore - 무시할 액터들
        OverlappingEnemys        // OutActors - 결과를 담을 배열
    );


    // 찾은 액터들에 대해 인터페이스로 스턴 호출
    for (AActor* OverlapEnemy : OverlappingEnemys)
    {
        if (!OverlapEnemy) continue;

        if (OverlapEnemy->GetClass()->ImplementsInterface(UAIEventReceiver::StaticClass()))
        {
            // 인터페이스에 정의된 ApplyStun 호출
            IAIEventReceiver::Execute_ApplyStun(OverlapEnemy);
            //TPT_LOG(GALog, Log, TEXT("Applied stun to enemy: %s"), *OverlapEnemy->GetName());
        }
    }
}

void AThrowEMP::DisableGlitchTrap()
{
    // 글리치함정은 tag로 체크
    //TPT_LOG(GALog, Log, TEXT("AThrowEMP::DisableGlitchTrap()"));

    TArray<AActor*> OverlappingGlitchTraps;

    // 검사할 오브젝트 타입 설정 (WorldStatic => GlitchTrap 액터)
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    // 무시할 액터
    TArray<AActor*> ActorsToIgnore;
    TArray<AActor*> AllPlayerCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayerCharacters);
    for (AActor* PlayerCharacter : AllPlayerCharacters)
    {
        if (PlayerCharacter)
        {
            ActorsToIgnore.Add(PlayerCharacter);
            //TPT_LOG(GALog, Log, TEXT("Added PlayerCharacter to ignore list: %s"), *PlayerCharacter->GetName());
        }
    }

    // 디버깅
//#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
//    DrawDebugSphere(
//        GetWorld(),                        // 월드 컨텍스트
//        GetActorLocation(),                // 구의 중심 위치
//        GlitchTrapDisableRadius,           // 구의 반경
//        12,                                // 구체의 세분화 정도 (원형면의 세그먼트 수, 적절하게 조절)
//        FColor::Green,                     // 색상
//        false,                            // 지속 시간 (false면 한 프레임만)
//        GlitchTrapDisableDuration         // 지속 시간 (초)
//    );
//#endif

    // AAIBaseCharacter 클래스 필터를 사용해서 적 캐릭터만 검색
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),                       // WorldContextObject
        GetActorLocation(),              // SpherePos - 구의 중심 위치
        GlitchTrapDisableRadius,         // SphereRadius - 구의 반경
        ObjectTypes,                     
        AGlitchTrap::StaticClass(),  // ActorClassFilter - AOverlapObject 클래스만 필터링
        ActorsToIgnore,                 // ActorsToIgnore - 무시할 액터들
        OverlappingGlitchTraps          // OutActors - 결과를 담을 배열
    );


    // GlitchTrap 태그 붙은 AOverlapObject만 GE, CUE 비활성화

    for (AActor* OverlapActor : OverlappingGlitchTraps)
    {
        AGlitchTrap* GlitchTrap = Cast<AGlitchTrap>(OverlapActor);
        if (GlitchTrap && GlitchTrap->Tags.Contains(FName("GlitchTrap")))
        {
            GlitchTrap->bEnableEffectAndCue = false;
            GlitchTrap->DeactivateMentalDamage(GlitchTrapDisableDuration);

            // 일정 시간 뒤 다시 true로 복원
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                [GlitchTrap]()
                {
                    if (GlitchTrap)
                    {
                        GlitchTrap->bEnableEffectAndCue = true;
                        GlitchTrap->ActivateMentalDamage();
                        //TPT_LOG(GALog, Log, TEXT("GlitchTrap re-enabled: %s"), *GlitchTrap->GetName());
                    }
                },
                GlitchTrapDisableDuration,
                false
            );
        }
    }
}

