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

void UFocusTraceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UFocusTraceComponent, FocusedActor, COND_None, REPNOTIFY_Always);
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

void UFocusTraceComponent::SetStart(const FVector& Vector)
{
    Start = Vector;
}

void UFocusTraceComponent::SetDirection(const FVector& Vector)
{
    Direction = Vector;
}

void UFocusTraceComponent::SetCollisionType(ECollisionChannel CollisionChannel)
{
    CollisionType = CollisionChannel;
}

void UFocusTraceComponent::OnRep_FocusedActor()
{
    APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
    if (!Character) return;

    if (IsValid(PrevActor) && PrevActor != FocusedActor)
    {
        if (PrevActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
            IInteract::Execute_CanInteract(PrevActor, Character, false);
    }
    if (IsValid(FocusedActor))
    {
        if (FocusedActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            if (!IInteract::Execute_CanInteract(FocusedActor, Character, true))
            {
                FocusedActor = nullptr;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("FocusedActor is null or invalid in OnRep_FocusedActor"));
    }

    PrevActor = FocusedActor;
}

void UFocusTraceComponent::PerformTrace()
{
    AActor* Owner = GetOwner();
    NULLCHECK_RETURN_LOG(Owner, PlayerLog, Warning, );
    APawn* Pawn = Cast<APawn>(Owner);
    if (!Pawn->HasAuthority())
    {
        return;
    }
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

    AActor* NewFocusedActor = bHit ? Hit.GetActor() : nullptr;
	
    APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
    NULLCHECK_RETURN_LOG(Character, GALog, Warning, );

    // 중복 세팅 방지: 값이 변경됐을 때만 갱신
    if (FocusedActor != NewFocusedActor)
    {
        PrevActor = FocusedActor;
        FocusedActor = NewFocusedActor;
    }

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), Start, End, Hit.GetActor() == FocusedActor ? FColor::Blue : FColor::Silver, false, 1.0f, 0, 0.3f);
#endif
}
