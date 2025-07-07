// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Interaction.h"
#include "InteractHideObject.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API AInteractHideObject : public ABaseObject, public IInteraction
{
	GENERATED_BODY()
	
public:
    AInteractHideObject();

protected:
	virtual void BeginPlay() override;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) override;


  
};
