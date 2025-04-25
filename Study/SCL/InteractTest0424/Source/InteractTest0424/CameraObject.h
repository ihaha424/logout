// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "CameraObject.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTTEST0424_API ACameraObject : public AObjectBase
{
	GENERATED_BODY()
	
public:
	ACameraObject();

public:
	
    // 상호작용 영역 컴포넌트 (트리거)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USphereComponent> InteractionTrigger;
    
    // 메쉬 컴포넌트 (시각적 표현)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComponent;
    
    // 상호작용 컴포넌트 (기능)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UInteractiveComponent> InteractComp;
    
    // 소음 컴포넌트 (기능)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UNoiseComponent> NoiseComp;

    void Makenoise(float Loudness, APawn* NoiseInstigator, FVector NoiseLocation, float MaxRange, FName Tag);

protected:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                         bool bFromSweep, const FHitResult& SweepResult);

};
