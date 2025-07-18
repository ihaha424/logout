// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FocusTraceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API UFocusTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UFocusTraceComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    AActor* GetFocusedActor() const { return FocusedActor; }

    void SetStart(FVector& Vector);
    void SetDirection(FVector& Vector);
    void SetCollisionType(ECollisionChannel& CollisionChannel);

protected:
    virtual void BeginPlay() override;

private:
    void PerformTrace();

    UPROPERTY()
    AActor* FocusedActor = nullptr;

    UPROPERTY(EditAnywhere)
    float TraceDistance = 1000.f;

	FVector Start;
	FVector Direction;
	ECollisionChannel CollisionType;
};
