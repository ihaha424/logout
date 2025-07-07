// Fill out your copyright notice in the Description page of Project Settings.


#include "CardKey.h"
#include "SzComponents/ItemComponent.h"

ACardKey::ACardKey() : AItemObject()
{
	bCanInteract = true;
}

void ACardKey::OnInteractServer_Implementation(const APawn* Interactor)
{
	DestroyLogic();
}

bool ACardKey::CanInteract_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(bCanInteract);

	return bCanInteract;
}

void ACardKey::DestroyLogic_Implementation()
{
	Destroy(true);
}
