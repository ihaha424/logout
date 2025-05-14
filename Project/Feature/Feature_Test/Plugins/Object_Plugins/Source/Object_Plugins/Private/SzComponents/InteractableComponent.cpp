// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/InteractableComponent.h"
#include "SzObjects/BaseObject.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	UE_LOG(LogTemp, Log, TEXT("UInteractableComponent::UInteractableComponent()"));

	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("UInteractableComponent::BeginPlay()"));

	Super::BeginPlay();

}


void UInteractableComponent::Execute(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UInteractableComponent::Execute"));

	//ABaseObject* Owner = Cast<ABaseObject>(GetOwner());
	//if (!Owner) return;

	//if (Owner->bDestory == true)
	//{
	//	DeleteLogic();
	//}
}

void UInteractableComponent::DeleteLogic()
{
	UE_LOG(LogTemp, Log, TEXT("UInteractableComponent::DeleteLogic"));

	ABaseObject* Owner = Cast<ABaseObject>(GetOwner());
	if (!Owner) return;

	// InteractionCount ++
	InteractionCount++;
	UE_LOG(LogTemp, Warning, TEXT("[%s] Interactive Component Triggered (%d/%d)"),
		*Owner->GetName(),
		InteractionCount,
		MaxInteractionCount);

	// MaxInteractionCount destroy
	if (InteractionCount >= MaxInteractionCount)
	{
		Owner->Destroy(true);

		UE_LOG(LogTemp, Error, TEXT("[%s] Destroyed after %d interactions!"),
			*Owner->GetName(),
			MaxInteractionCount);
	}
}

