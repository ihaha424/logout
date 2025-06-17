// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDefaultController.h"
#include "Components/WidgetComponent.h"

APlayerDefaultController::APlayerDefaultController()
{
 
}

void APlayerDefaultController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}
