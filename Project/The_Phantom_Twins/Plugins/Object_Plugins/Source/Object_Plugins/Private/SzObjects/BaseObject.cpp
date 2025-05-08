// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/StaticMeshComponent.h"
#include "SzComponents/InteractableComponent.h"


// Sets default values
ABaseObject::ABaseObject()
{
	UE_LOG(LogTemp, Log, TEXT("ABaseObject::ABaseObject()"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("OverlapAll"));

}

void ABaseObject::BeginPlay()
{
    Super::BeginPlay();

    // If InteractComp is not set (e.g., not assigned via Blueprint)
    if (!InteractComp)
    {
        // Try to find the first component of type UInteractableComponent attached to this Actor
        InteractComp = FindComponentByClass<UInteractableComponent>();

        // Log a warning if no such component was found
        if (!InteractComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("No InteractableComponent found on %s"), *GetName());
        }
    }
}

void ABaseObject::OnInteract_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ABaseObject::OnInteract"));

    if (InteractComp)
    {
        InteractComp->Execute(Interactor);

        if (bDestory)
        {
            InteractComp->DeleteLogic();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractComp is nullptr!"));
    }
}
