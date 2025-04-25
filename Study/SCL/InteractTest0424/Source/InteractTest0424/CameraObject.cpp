// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "InteractiveComponent.h"
#include "NoiseComponent.h"

ACameraObject::ACameraObject()
{
    // 루트 컴포넌트로 상호작용 트리거 스피어 생성
    InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
    RootComponent = InteractionTrigger;
    InteractionTrigger->SetSphereRadius(100.0f);
    InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));
    InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACameraObject::OnOverlapBegin);

    // 메쉬 컴포넌트 추가
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // 상호작용 컴포넌트 추가
    InteractComp = CreateDefaultSubobject<UInteractiveComponent>(TEXT("InteractComp"));

    // 소음 컴포넌트 추가
    NoiseComp = CreateDefaultSubobject<UNoiseComponent>(TEXT("NoiseComp"));

    UE_LOG(LogTemp, Log, TEXT("ACameraObject 생성"));
}

void ACameraObject::Makenoise(float Loudness, APawn* NoiseInstigator, FVector NoiseLocation, float MaxRange, FName Tag)
{
    UE_LOG(LogTemp, Log, TEXT("ACameraObject::Makenoise"));

    AActor::MakeNoise(Loudness, NoiseInstigator, NoiseLocation, MaxRange, Tag);
}

void ACameraObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("ACameraObject::OnOverlapBegin"));

}
