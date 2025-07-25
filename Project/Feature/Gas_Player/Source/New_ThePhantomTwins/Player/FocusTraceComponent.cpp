// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FocusTraceComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "SzInterface/Interact.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

UFocusTraceComponent::UFocusTraceComponent()
{
    FocusedActor = nullptr;
    Start = FVector::ZeroVector;
    Direction = FVector::ZeroVector;
    CollisionType = ECC_Visibility;

    PrimaryComponentTick.bCanEverTick = true;
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

void UFocusTraceComponent::SetCollisionType(ECollisionChannel CollisionChannel)
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

    AActor* PrevActor = FocusedActor;
    FocusedActor = bHit ? Hit.GetActor() : nullptr;

    APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
    NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), Start, End, Hit.GetActor() == FocusedActor ? FColor::Blue : FColor::Silver, false, 1.0f, 0, 0.3f);
#endif

	if (PrevActor != nullptr && FocusedActor != PrevActor)
	{
        if (PrevActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            IInteract::Execute_CanInteract(PrevActor, Character, false);
        }
	}

	if (FocusedActor != nullptr)
	{
        if (FocusedActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            if (!IInteract::Execute_CanInteract(FocusedActor, Character, true))
            {
                FocusedActor = nullptr;
            }
        }
	}
}