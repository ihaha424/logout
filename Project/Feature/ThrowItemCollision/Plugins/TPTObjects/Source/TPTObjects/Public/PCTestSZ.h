// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCTestSZ.generated.h"

/**
 * 
 */
UCLASS()
class TPTOBJECTS_API APCTestSZ : public APlayerController
{
	GENERATED_BODY()
		
public:
    APCTestSZ();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
    TObjectPtr<class UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
    TObjectPtr<class UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    TObjectPtr<class AActor> NearestInteractableActor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    TObjectPtr<class AActor> PreviousInteractableActor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 200.0f;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void HandleInteractionInput();
    void FindNearestInteractableActor();
    void UpdateInteractableActorState(APawn* playerPawn);

	// NetWork
	UFUNCTION(Server, Reliable)
	void C2S_Interact(UObject* interact);
	void C2S_Interact_Implementation(UObject* interact);
};
