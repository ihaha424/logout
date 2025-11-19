// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowableItem.h"

#include "ApplyEMP.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Log/TPTLog.h"


AThrowableItem::AThrowableItem()
{
	bReplicates = true;
	bNetLoadOnClient = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetNotifyRigidBodyCollision(true);                    // 충돌 알림 켜기
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 충돌 설정
	Mesh->OnComponentHit.AddDynamic(this, &AThrowableItem::OnItemFell);
	RootComponent = Mesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1200.f; // 시작 속도(원하는 값으로)
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 포물선 회전 자동
	ProjectileMovement->ProjectileGravityScale = 1.f;     // 1=기본 중력, 낮게/높게 
}

void AThrowableItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
}

void AThrowableItem::OnItemFell(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit)
{
	// OtherActor가 월드의 바닥(예: WorldStatic이나 특정 클래스로 확인)인지 판별
	if (OtherActor && OtherActor != this)
	{
		// 1. 메쉬 비가시화 및 충돌 해제
		Mesh->SetVisibility(false, true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UWorld* World = GetWorld();
        NULLCHECK_RETURN_LOG(World, ItemLog, Warning, );
        FVector Center = GetActorLocation();
        float Radius = 300.f; // 3미터

        TArray<FOverlapResult> Overlaps;
        FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

        // 필요한 쿼리 설정: WorldDynamic, Pawn 등 적절히 조절
        FCollisionObjectQueryParams ObjectQueryParams;
        ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // Pawn 타입 검사
        ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 필요에 따라 추가

        bool bHasOverlap = World->OverlapMultiByObjectType(
            Overlaps,
            Center,
            FQuat::Identity,
            ObjectQueryParams,
            Sphere
        );
        if (bHasOverlap)
        {
            for (const FOverlapResult& Result : Overlaps)
            {
                AActor* TargetActor = Result.GetActor();
                if (TargetActor && TargetActor != this)
                {
                    // EMP 중지 인터페이스 구현 여부 체크
                    if (TargetActor->GetClass()->ImplementsInterface(UApplyEMP::StaticClass()))
                    {
                        // 인터페이스 함수 호출
                        IApplyEMP::Execute_OnEMPPause(TargetActor);
                    }
                }
            }
        }
        
		Destroy();
	}
}
