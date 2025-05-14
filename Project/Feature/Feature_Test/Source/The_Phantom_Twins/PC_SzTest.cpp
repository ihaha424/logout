// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_SzTest.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "SZComponents/Interaction.h"
#include "SZComponents/InteractableComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

APC_SzTest::APC_SzTest()
{
	PrimaryActorTick.bCanEverTick = true;
    UE_LOG(LogTemp, Warning, TEXT("TTTTTTTTTTTTTTTTTEst"));
}

void APC_SzTest::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent);

    if (EnhancedInputComp)
    {
        if (InteractAction)
        {
            EnhancedInputComp->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APC_SzTest::HandleInteractionInput);
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

void APC_SzTest::BeginPlay()
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

void APC_SzTest::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FindNearestInteractableActor();
}

void APC_SzTest::HandleInteractionInput()
{
    UE_LOG(LogTemp, Log, TEXT("E"));

    if (NearestInteractableActor)
    {
        if (NearestInteractableActor->GetClass()->ImplementsInterface(UInteraction::StaticClass()))
        {
            APawn* playerPawn = GetPawn();
            check(playerPawn);

            IInteraction::Execute_OnInteract(NearestInteractableActor, playerPawn);
        }
    }
}

void APC_SzTest::FindNearestInteractableActor()
{
    NearestInteractableActor = nullptr;
    float ClosestDistance = InteractionDistance;

    FVector PlayerLocation = GetPawn()->GetActorLocation();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {

        if (Actor->GetClass()->ImplementsInterface(UInteraction::StaticClass()))
        {
            float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                NearestInteractableActor = Actor;
            }
        }
    }

#if WITH_EDITOR
    if (NearestInteractableActor)
    {
        DrawDebugSphere(
            GetWorld(),
            NearestInteractableActor->GetActorLocation(),
            100.0f,
            12,
            FColor::Green,
            false,
            -1.0f,
            0,
            1.0f
        );
    }
#endif
}
