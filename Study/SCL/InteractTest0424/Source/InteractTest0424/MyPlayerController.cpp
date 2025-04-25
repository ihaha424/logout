// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "InteractiveComponent.h"
#include "NoiseComponent.h"
#include "ObjectBase.h"
#include <Kismet/GameplayStatics.h>
#include "DrawDebugHelpers.h"

//#include "GameFramework/Actor.h"
//#include "InteractionInterface.h"
//#include "Components/SphereComponent.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// E키 상호작용 바인딩
	InputComponent->BindAction("Interact", IE_Pressed, this, &AMyPlayerController::HandleInteractionInput);
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 상호작용 가능한 오브젝트 검색
	FindNearestInteractableActor();
}

void AMyPlayerController::HandleInteractionInput()
{
    if (NearestInteractableActor)
    {
        // 객체에 InteractiveComponent가 있는지 확인
        UInteractiveComponent* InteractComp = NearestInteractableActor->FindComponentByClass<UInteractiveComponent>();
        if (InteractComp)
        {
            // 상호작용 처리
            InteractComp->OnInteract(GetPawn());

            // 객체에 NoiseComponent가 있는지 확인
            UNoiseComponent* NoiseComp = NearestInteractableActor->FindComponentByClass<UNoiseComponent>();
            if (NoiseComp)
            {
                // 소음 발생
                NoiseComp->MakeNoise();
            }

            // 객체의 모든 컴포넌트 로깅
            TArray<UActorComponent*> Components;
            NearestInteractableActor->GetComponents(Components);

            UE_LOG(LogTemp, Warning, TEXT("--- %s Components Log ---"), *NearestInteractableActor->GetName());
            for (UActorComponent* Component : Components)
            {
                UE_LOG(LogTemp, Warning, TEXT("Component: %s, Class: %s"),
                    *Component->GetName(), *Component->GetClass()->GetName());
            }
        }
    }
}

void AMyPlayerController::FindNearestInteractableActor()
{
    NearestInteractableActor = nullptr;
    float ClosestDistance = InteractionDistance;

    // 플레이어 위치 가져오기
    FVector PlayerLocation = GetPawn()->GetActorLocation();

    // 모든 ObjectBase 액터 검색
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectBase::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        // 액터에 InteractiveComponent가 있는지 확인
        UInteractiveComponent* InteractComp = Actor->FindComponentByClass<UInteractiveComponent>();
        if (InteractComp)
        {
            // 거리 계산
            float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                NearestInteractableActor = Actor;
            }
        }
    }

    // 디버그용 상호작용 표시 (에디터에서만)
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
