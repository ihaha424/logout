// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplineActor.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineActor();

	UPROPERTY(VisibleAnywhere)
	USplineComponent* SplineComponent;
};
