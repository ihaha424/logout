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

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    AActor* GetFocusedActor() const { return FocusedActor; }

    void SetStart(const FVector& Vector);
    void SetDirection(const FVector& Vector);
    void SetCollisionType(ECollisionChannel CollisionChannel);

    UFUNCTION()
    void OnRep_FocusedActor();

    void PerformTrace();
protected:
    virtual void BeginPlay() override;

private:

    UPROPERTY(ReplicatedUsing = OnRep_FocusedActor)
    AActor* FocusedActor;
    UPROPERTY()
    AActor* PrevActor = nullptr;
    UPROPERTY(EditAnywhere)
    float TraceDistance = 1000.f;

	FVector Start;
	FVector Direction;
	ECollisionChannel CollisionType;
};
