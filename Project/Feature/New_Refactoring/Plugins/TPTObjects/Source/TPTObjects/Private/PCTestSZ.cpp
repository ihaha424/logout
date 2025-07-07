// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTestSZ.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "SzInterface/Interact.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"


APCTestSZ::APCTestSZ()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APCTestSZ::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent);

    if (EnhancedInputComp)
    {
        if (InteractAction)
        {
            EnhancedInputComp->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APCTestSZ::HandleInteractionInput);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Enhanced Input X"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Enhanced Input X"));
    }
}

void APCTestSZ::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Enhanced Input X"));
        }
    }
}

void APCTestSZ::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FindNearestInteractableActor();
}

void APCTestSZ::HandleInteractionInput()
{
    if (NearestInteractableActor)
    {
        if (NearestInteractableActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            APawn* playerPawn = GetPawn();

            if (playerPawn && IInteract::Execute_CanInteract(NearestInteractableActor, playerPawn, true))
            {
                IInteract::Execute_OnInteractServer(NearestInteractableActor, playerPawn);
            }
        }
    }
}

void APCTestSZ::FindNearestInteractableActor()
{
    NearestInteractableActor = nullptr;
    float ClosestDistance = InteractionDistance;

    FVector PlayerLocation = GetPawn()->GetActorLocation();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    APawn* playerPawn = GetPawn();

    if (!playerPawn) return;

    // 1. 가장 가까운 인터랙터블 액터 찾기
    for (AActor* Actor : FoundActors)
    {
        if (Actor && Actor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                NearestInteractableActor = Actor;
            }
        }
    }

    // 2. 모든 Actor에 대해 CanInteract 호출
    for (AActor* Actor : FoundActors)
    {
        if (Actor && Actor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            bool bIsNearest = (Actor == NearestInteractableActor);
            IInteract::Execute_CanInteract(Actor, playerPawn, bIsNearest);
        }
    }

//#if WITH_EDITOR
//    if (NearestInteractableActor)
//    {
//        DrawDebugSphere(
//            GetWorld(),
//            NearestInteractableActor->GetActorLocation(),
//            100.0f,
//            12,
//            FColor::Green,
//            false,
//            -1.0f,
//            0,
//            1.0f
//        );
//    }
//#endif
}