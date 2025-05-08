// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "HideComponent.generated.h"


UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UHideComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHideComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void Execute(APawn* Interactor) override;
		
};
