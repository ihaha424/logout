// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerHideObject.h"
#include "Components/BoxComponent.h"

ATriggerHideObject::ATriggerHideObject()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
    BoxTriggerComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    BoxTriggerComponent->SetGenerateOverlapEvents(true);

}

void ATriggerHideObject::BeginPlay()
{
    Super::BeginPlay();

    if (BoxTriggerComponent)
    {
        BoxTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerBeginOverlap);
        BoxTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerEndOverlap);
    }
}

void ATriggerHideObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    HideStatus.PlayerNum = FMath::Clamp(HideStatus.PlayerNum + 1, 0, 2);
    HideStatus.bHasPlayer = (HideStatus.PlayerNum > 0);
}

void ATriggerHideObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    HideStatus.PlayerNum = FMath::Clamp(HideStatus.PlayerNum - 1, 0, 2);
    HideStatus.bHasPlayer = (HideStatus.PlayerNum > 0);
}
