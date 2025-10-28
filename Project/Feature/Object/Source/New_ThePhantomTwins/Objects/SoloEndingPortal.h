// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SoloEndingPortal.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ASoloEndingPortal : public AInteractableObject
{
	GENERATED_BODY()
	
public:
	ASoloEndingPortal();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;


};
