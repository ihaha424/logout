// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseComponent.generated.h"

USTRUCT(BlueprintType)
struct FNoiseSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
    float Loudness = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise Settings")
    APawn* NoiseInstigator = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise Settings")
    FVector NoiseLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
    FVector NoiseOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
    float MaxRange = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
    FName Tag = NAME_None;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTTEST0424_API UNoiseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoiseComponent();

public:
	// 소음 발생 함수
    UFUNCTION(BlueprintCallable, Category = "Noise")
    void MakeNoise();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
    FNoiseSettings NoiseSettings;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
