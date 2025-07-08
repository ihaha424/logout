// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"

AItemObject::AItemObject() : ABaseObject()
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

bool AItemObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return false;

	bCanInteract = bIsDetected;

	UE_LOG(LogTemp, Log,
		TEXT("AItemObject::CanInteract - %s | %s | Role: %s"),
		bCanInteract ? TEXT("true") : TEXT("false"),
		*Interactor->GetName(),
		*UEnum::GetValueAsString(GetLocalRole()));

	SetWidgetVisible(bCanInteract);

	return bCanInteract;
}

void AItemObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("AItemObject::OnInteractServer"));

	UseItemEffectServer();
}

void AItemObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("AItemObject::OnInteractClient"));
	
	UseItemEffectClient();
}

void AItemObject::UseItemEffectServer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AItemObject::UseItemEffect Server"));
}

void AItemObject::UseItemEffectClient_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AItemObject::UseItemEffect Client"));
}
