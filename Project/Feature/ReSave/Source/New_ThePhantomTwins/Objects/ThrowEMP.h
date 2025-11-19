// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ThrowEMP.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API AThrowEMP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowEMP();

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

    // 폭발/충돌 처리
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, 
               const FHitResult& Hit);

    // 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
    void InvokeGameplayCue();


private:
    void ExplodeAndMakeNoise();

    // 적에게 닿으면 n초간 스턴
    void ApplyStunToEnemy();

    // 글리치함정에 닿으면 n초간 비활성화
    void DisableGlitchTrap();

public:
    FTimerHandle DestroyTimer;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components") 
    TObjectPtr<class USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EMP|Enemy")
    float EnemyStunRadius = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EMP|Enemy")
    float EnemyStunDuration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EMP|GlitchTrap")
    float GlitchTrapDisableRadius = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EMP|GlitchTrap")
    float GlitchTrapDisableDuration = 10.0f;

    UPROPERTY(EditAnywhere, Category = "EMP", Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;

};
