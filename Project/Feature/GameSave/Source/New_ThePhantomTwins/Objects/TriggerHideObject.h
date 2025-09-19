// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Destroyable.h"
#include "TriggerHideObject.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ATriggerHideObject : public ABaseObject, public IDestroyable
{
	GENERATED_BODY()
	
public:	
	ATriggerHideObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	// AI percrption(적이 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | AI")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | AI")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide")
	TObjectPtr<class UBoxComponent> BoxTriggerComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide", Replicated)
	bool bHasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Hide", Replicated)
	int32 HidePlayerNum = 0;
};
