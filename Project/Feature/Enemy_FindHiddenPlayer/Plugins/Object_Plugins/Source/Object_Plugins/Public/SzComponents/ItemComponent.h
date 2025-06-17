// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzComponents/InteractableComponent.h"
#include "ItemComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UItemComponent : public UInteractableComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void ExecuteSever(APawn* Interactor) override;
};
