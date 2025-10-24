// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "ScanEffectActor.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AScanEffectActor : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AScanEffectActor();

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
};
