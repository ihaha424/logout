// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"

ASplineActor::ASplineActor()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("DefaultSplineComponent"));
	RootComponent = SplineComponent;
}
