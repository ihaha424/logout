// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FocusTraceComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Log/TPTLog.h"

UFocusTraceComponent::UFocusTraceComponent()
{
    FocusedActor = nullptr;
    Start = FVector::ZeroVector;
    Direction = FVector::ZeroVector;
    CollisionType = ECC_Visibility;
}

void UFocusTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFocusTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformTrace();
}

void UFocusTraceComponent::SetStart(FVector& Vector)
{
    Start = Vector;
}

void UFocusTraceComponent::SetDirection(FVector& Vector)
{
    Direction = Vector;
}

void UFocusTraceComponent::SetCollisionType(ECollisionChannel& CollisionChannel)
{
    CollisionType = CollisionChannel;
}

void UFocusTraceComponent::PerformTrace()
{
    AActor* Owner = GetOwner();
    NULLCHECK_RETURN_LOG(Owner, PlayerLog, Warning, )

    FVector End = Start + Direction * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner); // 자기자신은 무시

    bool bHit = GetWorld()->LineTraceSingleByChannel
    (Hit, 
    Start,
    End, 
    CollisionType, 
    Params);

    FocusedActor = bHit ? Hit.GetActor() : nullptr;

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), Start, End, Hit.GetActor() == FocusedActor ? FColor::Blue : FColor::Silver, false, 1.0f, 0, 0.3f);
#endif
}