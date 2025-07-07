// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"

AInteractHideObject::AInteractHideObject() : ABaseObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractHideObject::BeginPlay()
{
	Super::BeginPlay();

}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AInteractHideObject, );
}

void AInteractHideObject::OnInteractServer_Implementation(const APawn* Interactor)
{
}

bool AInteractHideObject::CanInteract_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(bCanInteract);

	return bCanInteract;
}