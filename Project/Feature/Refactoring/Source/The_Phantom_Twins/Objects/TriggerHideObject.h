// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SzObjects/ObjectState.h"
#include "TriggerHideObject.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API ATriggerHideObject : public AActor
{
	GENERATED_BODY()
	
public:
    ATriggerHideObject();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject")
	TObjectPtr<class UBoxComponent> BoxTriggerComponent;

     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject")
    FHideStatus HideStatus;

};
