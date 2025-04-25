// Fill out your copyright notice in the Description page of Project Settings.


#include "CabinetObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "InteractiveComponent.h"

ACabinetObject::ACabinetObject()
{
    // 루트 컴포넌트로 상호작용 트리거 스피어 생성
    InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
    RootComponent = InteractionTrigger;
    InteractionTrigger->SetSphereRadius(200.0f);
    InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));
    InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACabinetObject::OnOverlapBegin);

    // 메쉬 컴포넌트 추가
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // 상호작용 컴포넌트만 추가 (소음 컴포넌트 없음)
    InteractComp = CreateDefaultSubobject<UInteractiveComponent>(TEXT("InteractComp"));

    UE_LOG(LogTemp, Log, TEXT("ACabinetObject 생성"));
}


void ACabinetObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 오버랩 시 로직 (필요시 구현)
    UE_LOG(LogTemp, Log, TEXT("ACabinetObject::OnOverlapBegin"));

    MakeNoise();
}
