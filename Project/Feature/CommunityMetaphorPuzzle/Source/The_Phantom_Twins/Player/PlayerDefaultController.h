// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerDefaultController.generated.h"

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

	UFUNCTION(Server, Reliable)
	void C2S_SetOwnerActor(APlayerController* thisPC, AActor* Actor);
	void C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor);
};
