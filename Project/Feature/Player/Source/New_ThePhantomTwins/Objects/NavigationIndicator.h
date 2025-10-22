// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "NavigationIndicator.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ANavigationIndicator : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
    ANavigationIndicator();

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* IndicatorMesh; // 또는 UNiagaraComponent

    // 사용자 조정 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    float OrbitRadius = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    float OrbitSpeedDegPerSec = 120.f;     // 초당 각도(도)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    float HeightOffset = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    bool  bAlsoSpinSelf = true;            // 인디케이터 자체 자전

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    FVector  TargetPosition;            // Target Posotion

    virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
    virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
    virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

    UFUNCTION(BlueprintCallable, Category = "Indicator")
    void RotationToTargetActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indicator")
    TWeakObjectPtr<AActor> TargetCached;
private:

    float AngleDeg = 0.f;
};
