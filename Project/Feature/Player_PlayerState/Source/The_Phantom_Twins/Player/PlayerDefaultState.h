// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerDefaultState.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNoiseInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunNoise = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkNoise = 0.0f;
};

USTRUCT(BlueprintType)
struct FMoveSpeedInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchSpeed = 80.f;
};

UCLASS()
class THE_PHANTOM_TWINS_API APlayerDefaultState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APlayerDefaultState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Player Info")
	FMoveSpeedInfo MoveSpeedInfo;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Player Info")
	FNoiseInfo NoiseInfo;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Groggy, BlueprintReadOnly, Category = "Player Info")
	bool bIsGroggy = false;

public:

	UFUNCTION()
	void OnRep_Groggy();
};
