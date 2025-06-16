// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDefaultController.h"
#include "Components/WidgetComponent.h"

APlayerDefaultController::APlayerDefaultController()
{
 
}

void APlayerDefaultController::C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor)
{
	if(HasAuthority())
		Actor->SetOwner(thisPC);
}
