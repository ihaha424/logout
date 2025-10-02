// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "ThrowNoiseBomb.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API AThrowNoiseBomb : public APawn
{
	GENERATED_BODY()

public:
    AThrowNoiseBomb();

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

    // 폭발/충돌 처리
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);


    // 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
    void InvokeGameplayCue();

private:
    void ExplodeAndMakeNoise();

    // 특정 액터에 붙을 수 있는지 확인
    bool CanStickToActor(AActor* Actor);

    // 표면에 붙는 처리
    void StickToSurface(AActor* SurfaceActor, const FHitResult& HitResult);

    // 바닥으로 떨어뜨리는 처리
    void FallToGround();

    void DestroyNoiseBomb();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components") 
    TObjectPtr<class USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UNoiseComponent> NoiseComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float NoiseDuration = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Noise", Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;

private:
    UPROPERTY()
    FTimerHandle GroundCheckTimer;
};
