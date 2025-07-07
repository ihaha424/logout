// Fill out your copyright notice in the Description page of Project Settings.


#include "SzInterface/Hacking.h"

// Add default functionality here for any IHacking functions that are not pure virtual.

void IHacking::OnInteractServer_Implementation(const APawn* Interactor)
{
	OnHackingStartedServer(Interactor);
}

void IHacking::OnInteractClient_Implementation(const APawn* Interactor)
{
	OnHackingStartedClient(Interactor);
}

bool IHacking::CanInteract_Implementation(const APawn* Interactor)
{
	return CanBeHacked(Interactor);
}
