// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NoiseBomb.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ANoiseBomb : public APawn
{
	GENERATED_BODY()

public:
    ANoiseBomb();

public:
    virtual void BeginPlay() override;

    // 气惯/面倒 贸府
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);

private:
    void ExplodeAndMakeNoise();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components") 
    TObjectPtr<class USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UNoiseComponent> NoiseComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NoiseBomb")
    float NoiseDuration = 10.0f;

};
