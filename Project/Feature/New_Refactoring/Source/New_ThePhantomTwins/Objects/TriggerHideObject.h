// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SzObjects/ObjectState.h"
#include "TriggerHideObject.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ATriggerHideObject : public AActor
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
	// AI percrption(적이 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | AI")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Components")
	TObjectPtr<class USceneComponent> RootSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Components")
	TObjectPtr<class UOutlineComponent> OutlineComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide")
	TObjectPtr<class UBoxComponent> BoxTriggerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide")
	bool bHasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide")
	int32 HidePlayerNum = 0;
};
