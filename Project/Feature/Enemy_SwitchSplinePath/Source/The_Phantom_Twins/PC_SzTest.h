// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_SzTest.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API APC_SzTest : public APlayerController
{
	GENERATED_BODY()
	
public:
	APC_SzTest();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    TObjectPtr<class UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    TObjectPtr<class UInputAction> InteractAction;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<class AActor> NearestInteractableActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 250.0f;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void HandleInteractionInput();

    void FindNearestInteractableActor();

};
