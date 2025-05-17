// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/InteractableComponent.h"
#include "SzObjects/BaseObject.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UInteractableComponent::Execute(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UInteractableComponent::Execute: Please Ovrride."));
	//ABaseObject* Owner = Cast<ABaseObject>(GetOwner());
	//if (!Owner) return;

	//if (Owner->bDestory == true)
	//{
	//	DeleteLogic();
	//}
}

void UInteractableComponent::DeleteLogic()
{
	ABaseObject* Owner = Cast<ABaseObject>(GetOwner());
	if (!Owner) return;

	// InteractionCount ++
	InteractionCount++;
	// MaxInteractionCount destroy
	if (InteractionCount >= MaxInteractionCount)
	{

		Owner->Destroy(true);
	}
}

