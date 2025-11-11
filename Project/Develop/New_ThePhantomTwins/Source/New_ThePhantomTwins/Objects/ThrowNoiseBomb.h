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
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 폭발/충돌 처리
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);


    // 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
    void InvokeGameplayCue();

    // 블루프린트에서 사운드 출력 구현할 수 있도록 선언
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Noise")
	void PlayNoiseBombSound();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Noise")
	void StopNoiseBombSound();

private:
    void ExplodeAndMakeNoise();

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

    UPROPERTY()
    FTimerHandle DestroyTimer;

};
