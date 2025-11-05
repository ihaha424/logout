// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FocusTraceComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
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

void UFocusTraceComponent::SetStartOfsset(const float Offset)
{
	StartOffset = Offset;
}

void UFocusTraceComponent::OnRep_FocusedActor()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
	if (!Character) return;

	if (!Character->IsLocallyControlled()) return;

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

	PrevActor = FocusedActor;
}

void UFocusTraceComponent::PerformTrace()
{
    AActor* Owner = GetOwner();
    NULLCHECK_RETURN_LOG(Owner, PlayerLog, Warning, );
	APlayerCharacter* Character = Cast<APlayerCharacter>(Owner);
    NULLCHECK_RETURN_LOG(Character, PlayerLog, Warning, );
    APawn* Pawn = Cast<APawn>(Owner);
    if (!Pawn->HasAuthority())
    {
        /*APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
		if (!Character) return;

		if (!Character->IsLocallyControlled()) return;
        if (IsValid(FocusedActor))
        {
			if (FocusedActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
            {
				if(!IInteract::Execute_CanInteract(FocusedActor, Character, true))
				{
					FocusedActor = nullptr;
				}
            }
        }*/
        return;
    }
    FVector End = Start + Direction * TraceDistance;
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner); // 자기자신은 무시
    Params.AddIgnoredComponent(Character->BoxComp);

    FCollisionObjectQueryParams ObjParams;
    ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
 
    FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);

    bool bHit = GetWorld()->SweepSingleByObjectType(
        Hit,
        Start + (Direction * StartOffset),
        End,
        FQuat::Identity, // 회전 필요 없으면 Identity
        ObjParams,
        Sphere,
        Params
    );

    AActor* NewFocusedActor = bHit ? Hit.GetActor() : nullptr;

	PrevActor = FocusedActor;
	FocusedActor = NewFocusedActor;


#if WITH_EDITOR
    if (bOnDebug)
    {
		DrawDebugSphere(
			GetWorld(),
			Hit.ImpactPoint,
			SphereRadius,
			16,
			(Hit.GetActor() == FocusedActor ? FColor::Yellow : FColor::Silver),
			false,
			1.0f,
			0,
			1.0f
		);
    }
#endif

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
    
}
