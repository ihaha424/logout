// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActorManager.h"
#include "SplineActor.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogSplineActor);

/*
// QuadTreeNode with ASplineActor.
*/

QuadTreeNode::QuadTreeNode(FVector2D InOrigin, float InHalfSize, int32 InDepth)
    : Origin(InOrigin), HalfSize(InHalfSize), Depth(InDepth)
{
    for (int i = 0; i < 4; ++i)
        Children[i] = nullptr;
}

void QuadTreeNode::Insert(const FVector2D& Position, ASplineActor* Actor)
{
    if (!FMath::IsWithin(Position.X, Origin.X - HalfSize, Origin.X + HalfSize) ||
        !FMath::IsWithin(Position.Y, Origin.Y - HalfSize, Origin.Y + HalfSize))
    {
        return; // out of bounds
    }

    if (!StoredActor && !Children[0])
    {
        StoredPosition = Position;
        StoredActor = Actor;
        return;
    }

    if (!Children[0])
    {
        float Quarter = HalfSize / 2;
        Children[0] = MakeUnique<QuadTreeNode>(FVector2D(Origin.X - Quarter, Origin.Y - Quarter), Quarter, Depth + 1);
        Children[1] = MakeUnique<QuadTreeNode>(FVector2D(Origin.X + Quarter, Origin.Y - Quarter), Quarter, Depth + 1);
        Children[2] = MakeUnique<QuadTreeNode>(FVector2D(Origin.X - Quarter, Origin.Y + Quarter), Quarter, Depth + 1);
        Children[3] = MakeUnique<QuadTreeNode>(FVector2D(Origin.X + Quarter, Origin.Y + Quarter), Quarter, Depth + 1);

        // reinsert existing actor
        if (StoredActor)
        {
            for (auto& Child : Children)
                Child->Insert(StoredPosition, StoredActor);

            StoredActor = nullptr;
        }
    }

    for (auto& Child : Children)
    {
        Child->Insert(Position, Actor);
    }
}

ASplineActor* QuadTreeNode::QueryNearest(const FVector2D& Point, float& OutDistSq) const
{
    ASplineActor* Nearest = nullptr;

    if (FMath::Abs(Point.X - Origin.X) > HalfSize + FMath::Sqrt(OutDistSq) ||
        FMath::Abs(Point.Y - Origin.Y) > HalfSize + FMath::Sqrt(OutDistSq))
    {
        return nullptr;
    }

    if (StoredActor)
    {
        float DistSq = FVector2D::DistSquared(Point, StoredPosition);
        if (DistSq < OutDistSq)
        {
            OutDistSq = DistSq;
            Nearest = StoredActor;
        }
    }

    if (Children[0])
    {
        for (const auto& Child : Children)
        {
            ASplineActor* Candidate = Child->QueryNearest(Point, OutDistSq);
            if (Candidate)
            {
                Nearest = Candidate;
            }
        }
    }

    return Nearest;
}





/*
// ASplineActorManager.
*/
ASplineActorManager* ASplineActorManager::SingletonInstance = nullptr;

ASplineActorManager::ASplineActorManager()
{
    PrimaryActorTick.bCanEverTick = false;
    RootNode = MakeUnique<QuadTreeNode>(FVector2D::ZeroVector, QuadTreeSize, 0);
}

void ASplineActorManager::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASplineActorManager::StaticClass(), Found);
    if (Found.Num() > 1)
    {
        UE_LOG(LogSplineActor, Error, TEXT("Multiple SplineActorManagers found! Destroying this one."));
        Destroy();
        return;
    }
}

void ASplineActorManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SingletonInstance == this)
    {
        SingletonInstance = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void ASplineActorManager::RegisterSpline(const FVector2D& Location, ASplineActor* SplineActor)
{
    if (SplineActor)
    {
        RootNode->Insert(Location, SplineActor);
    }
}

ASplineActor* ASplineActorManager::FindNearestSpline(const FVector2D& WorldLocation) const
{
    // 초기 탐색 반경
    float SearchRadius = 500.0f;
    ASplineActor* Closest = nullptr;
    float ClosestDistSq = TNumericLimits<float>::Max();

    while (!Closest && SearchRadius < QuadTreeSize * 2)
    {
        Closest = nullptr;
        ClosestDistSq = TNumericLimits<float>::Max();
        Closest = RootNode->QueryNearest(WorldLocation, ClosestDistSq);

        // 점점 확장
        SearchRadius *= 2;
    }

    return Closest;
}

ASplineActorManager* ASplineActorManager::Get(UWorld* World)
{
    if (!SingletonInstance || !IsValid(SingletonInstance))
    {
        SingletonInstance = Cast<ASplineActorManager>(UGameplayStatics::GetActorOfClass(World, StaticClass()));
    }
    return SingletonInstance;
}
