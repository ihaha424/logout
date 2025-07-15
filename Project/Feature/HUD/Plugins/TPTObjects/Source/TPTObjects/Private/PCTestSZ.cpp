// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTestSZ.h"
#include "Net/UnrealNetwork.h"
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
            UE_LOG(LogTemp, Error, TEXT("Enhanced Input X"));
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

    APawn* MyPawn = GetPawn();
    if (!MyPawn) return;

    if (APlayerController* PC = Cast<APlayerController>(MyPawn->Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Enhanced Input X"));
        }
    }
}

void APCTestSZ::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* playerPawn = GetPawn();
    if (!playerPawn) return;
    if (!playerPawn->IsLocallyControlled()) return;


    // 1. 가장 가까운 인터랙터블 액터 찾기
    FindNearestInteractableActor();

    // 2. 이전과 현재가 다르면 상태 변경
    UpdateInteractableActorState(playerPawn);

}

void APCTestSZ::HandleInteractionInput()
{
    if (NearestInteractableActor && NearestInteractableActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
    {
        APawn* playerPawn = GetPawn();

        if (playerPawn && IInteract::Execute_CanInteract(NearestInteractableActor, playerPawn, true))
        {
            C2S_Interact(NearestInteractableActor);
            IInteract::Execute_OnInteractClient(NearestInteractableActor, playerPawn);
        }
    }
}

void APCTestSZ::FindNearestInteractableActor()
{
    float ClosestDistance = InteractionDistance;
    AActor* CurrNearActor = nullptr;

    APawn* playerPawn = GetPawn();
    if (!playerPawn) return;

    FVector PlayerLocation = playerPawn->GetActorLocation();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (Actor && Actor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                CurrNearActor = Actor;
            }
        }
    }

    NearestInteractableActor = CurrNearActor;

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


void APCTestSZ::UpdateInteractableActorState(APawn* playerPawn)
{
    if (PreviousInteractableActor != NearestInteractableActor)
    {
        // 이전 액터의 위젯 끄기
		if (PreviousInteractableActor && PreviousInteractableActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
		{
            UE_LOG(LogTemp, Log,
                TEXT("Prev Actor( %s ) Visible False | %s | Role: %s"),
                *PreviousInteractableActor->GetName(),
                *GetName(),
                *UEnum::GetValueAsString(GetLocalRole()));

            IInteract::Execute_CanInteract(PreviousInteractableActor, playerPawn, false);
        }

        // 현재 액터의 위젯 켜기
        if (NearestInteractableActor && NearestInteractableActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
        {
            UE_LOG(LogTemp, Log,
                TEXT("Curr Actor( %s ) Visible True | %s | Role: %s"),
                *NearestInteractableActor->GetName(),
                *GetName(),
                *UEnum::GetValueAsString(GetLocalRole()));

            IInteract::Execute_CanInteract(NearestInteractableActor, playerPawn, true);
        }

        PreviousInteractableActor = NearestInteractableActor;
    }
}

void APCTestSZ::C2S_Interact_Implementation(UObject* interact)
{
    if (nullptr == interact)
    {
        return;
    }

    APawn* playerPawn = GetPawn();

    if (interact->GetClass()->ImplementsInterface(UInteract::StaticClass()))
    {
        IInteract::Execute_OnInteractServer(interact, playerPawn);
    }
}