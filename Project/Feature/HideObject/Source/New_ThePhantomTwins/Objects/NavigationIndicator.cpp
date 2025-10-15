// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/NavigationIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

ANavigationIndicator::ANavigationIndicator()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoDestroyOnRemove = true;
    bAutoAttachToOwner = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    IndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IndicatorMesh"));
    IndicatorMesh->SetupAttachment(RootComponent);

    bReplicates = false;
    SetReplicateMovement(false);
}

bool ANavigationIndicator::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    TargetCached = MyTarget;

    if (bAutoAttachToOwner && MyTarget)
    {
        AttachToActor(MyTarget, FAttachmentTransformRules::KeepRelativeTransform);
    }

    // 시작 위치: 반지름만큼 X축으로 띄우고 높이 오프셋
    //SetActorRelativeLocation(FVector(OrbitRadius, 0, HeightOffset));
    //AngleDeg = 0.f;

    // 필요하면 나이아가라/사운드 켜기
    return true;
}

bool ANavigationIndicator::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    SetActorTickEnabled(true);
   
    return true;
}

bool ANavigationIndicator::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    SetActorTickEnabled(false);

    // 나이아가라/사운드 끄기 등
    return true;
}

void ANavigationIndicator::RotationToTargetActor()
{
    const float DT = GetWorld()->GetDeltaSeconds();
    AngleDeg = FMath::Fmod(AngleDeg + OrbitSpeedDegPerSec * DT, 360.f);

    // 궤도 위치 갱신 (상대 위치)
    //const float Rad = FMath::DegreesToRadians(AngleDeg);
    //const float X = OrbitRadius * FMath::Cos(Rad);
    //const float Y = OrbitRadius * FMath::Sin(Rad);
    //SetActorRelativeLocation(FVector(X, Y, HeightOffset));

    // === 여기부터 추가: Position(월드)을 바라보게 ===
    const FVector MyWorldLoc = GetActorLocation();     // 현재 액터의 월드 위치
    FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(MyWorldLoc, TargetPosition);

    // 상하 기울어짐 방지(선택)
    LookRot.Pitch = 0.f;
    LookRot.Roll = 0.f;

    SetActorRotation(LookRot);

    // 필요시 추가 자전(원래 코드 유지)
    //if (bAlsoSpinSelf)
    //{
    //    AddActorLocalRotation(FRotator(0.f, OrbitSpeedDegPerSec * DT, 0.f));
    //}
}
