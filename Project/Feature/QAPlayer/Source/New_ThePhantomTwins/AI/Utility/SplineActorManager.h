// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineActorManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSplineActor, Log, All);

class ASplineActor;

/**
 * @brief
        : If create a template when used elsewhere.
 */
class QuadTreeNode
{
public:
    FVector2D Origin;
    float HalfSize;
    int32 Depth;

    FVector2D StoredPosition;
    ASplineActor* StoredActor = nullptr;

    TUniquePtr<QuadTreeNode> Children[4];

    QuadTreeNode() {}
    QuadTreeNode(FVector2D InOrigin, float InHalfSize, int32 InDepth);

    void Insert(const FVector2D& Position, ASplineActor* Actor);
    ASplineActor* QueryNearest(const FVector2D& Point, float& OutDistSq) const;
};

UCLASS()
class NEW_THEPHANTOMTWINS_API ASplineActorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineActorManager();

    UFUNCTION(BlueprintCallable, Category = "Spline")
    static ASplineActorManager* Get(UWorld* World);

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void RegisterSpline(const FVector2D& Location, ASplineActor* SplineActor);

    ASplineActor* FindNearestSpline(const FVector2D& WorldLocation) const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
    float QuadTreeSize = 12000.f;

private:
    TUniquePtr<QuadTreeNode> RootNode;
private:
    static ASplineActorManager* SingletonInstance;
};
