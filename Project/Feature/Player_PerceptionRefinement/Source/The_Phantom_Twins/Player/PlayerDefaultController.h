// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AIPerceptionTypes.h"
#include "PlayerDefaultController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API APlayerDefaultController : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerDefaultController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> Perception;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TArray<AActor*> PerceptionActors;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// Network
	UFUNCTION(Client, Reliable)
	void S2C_UpdatePerceivedActor(AActor* Actor, bool bVisible);
	void S2C_UpdatePerceivedActor_Implementation(AActor* Actort, bool bVisible);

	TArray<AActor*> PerceptionActors;
};
