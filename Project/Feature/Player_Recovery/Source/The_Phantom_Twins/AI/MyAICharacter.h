// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SplinePathActor.h"
#include "SzInterface/Hacking.h"
#include "MyAICharacter.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API AMyAICharacter : public ACharacter, public IHacking
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyAICharacter();

	virtual void OnHackingStarted_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Spline")
	ASplinePathActor* SplinePath;

	float DistanceAlongSpline = 0.f;
	bool bMovingForward = true;

	UPROPERTY(EditAnywhere, Category = "AI|Spline")
	float MoveSpeed = 200.f;

	bool bIsFollowingSpline = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	ASplinePathActor* GetSplinePath() const { return SplinePath; }
};
