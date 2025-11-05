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

    UFUNCTION(BlueprintCallable)
    void SetStart(const FVector& Vector);
    UFUNCTION(BlueprintCallable)
    void SetDirection(const FVector& Vector);
    UFUNCTION(BlueprintCallable)
    void SetCollisionType(ECollisionChannel CollisionChannel);

    UFUNCTION(BlueprintCallable)
    void SetStartOfsset(const float Offset);

    UFUNCTION()
    void OnRep_FocusedActor();

    void PerformTrace();

    // 라인의 두께
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SphereRadius = 50.f;
	UPROPERTY(ReplicatedUsing = OnRep_FocusedActor)
	AActor* FocusedActor;
protected:
    virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartOffset = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bOnDebug = false;

private:

    UPROPERTY()
    AActor* PrevActor = nullptr;

	FVector Start;
	FVector Direction;
	ECollisionChannel CollisionType;
};
