// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AIScanner.generated.h"

class ULightComponent;

USTRUCT()
struct FLightState
{
	GENERATED_BODY()
	float OriginalIntensity = 0.f;
	int32 OverlapCount = 0;
};


UCLASS()
class NEW_THEPHANTOMTWINS_API AAIScanner : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	virtual bool MatchingChaseActorType(AActor* OtherActor) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void UpdateNearbySpotLights();

protected:
	UPROPERTY()
	float LightAffectRadius = 2000.f;

	UPROPERTY()
	TMap<TWeakObjectPtr<ULightComponent>, FLightState> ManagedLights;
};
