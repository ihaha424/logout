// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/HideComponent.h"

// Sets default values for this component's properties
UHideComponent::UHideComponent()
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::UHideComponent()"));

	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHideComponent::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::BeginPlay()"));

	Super::BeginPlay();
}

void UHideComponent::Execute(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::Execute"));
}
