// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"

AItemObject::AItemObject() : AInteractableObject()
{
	bReplicates = true;

}

void AItemObject::BeginPlay()
{
	Super::BeginPlay();

}

void AItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AItemObject, );
}

void AItemObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	UseItemEffectServer();
}

void AItemObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	Super::OnInteractClient(Interactor);

	UseItemEffectClient();
}

void AItemObject::UseItemEffectServer_Implementation()
{
}

void AItemObject::UseItemEffectClient_Implementation()
{
}
