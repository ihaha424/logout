// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/ItemObject.h"
#include "SzInterface/Interaction.h"
#include "CardKey.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API ACardKey : public AItemObject, public IInteraction
{
	GENERATED_BODY()
	
public:
	ACardKey();

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) override;

	virtual void DestroyLogic_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardKey")
	int32 grade = 0;
};
