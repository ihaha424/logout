// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"
#include "SplineActorManager.h"

ASplineActor::ASplineActor()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("DefaultSplineComponent"));
	RootComponent = SplineComponent;
}

void ASplineActor::BeginPlay()
{
    Super::BeginPlay();

    if (ASplineActorManager* Manager = ASplineActorManager::Get(GetWorld()))
    {
        int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
        for (int32 i = 0; i < NumPoints; ++i)
        {
            FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
            FVector2D PointLocation2D = FVector2D(PointLocation.X, PointLocation.Y);
            Manager->RegisterSpline(PointLocation2D, this);
        }
    }
}
