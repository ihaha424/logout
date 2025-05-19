// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Execute(APawn* Interactor);
	virtual void DeleteLogic();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    int32 MaxInteractionCount = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    int32 InteractionCount = 0;
};
