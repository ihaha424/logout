// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/ItemComponent.h"

UItemComponent::UItemComponent()
{
	UE_LOG(LogTemp, Log, TEXT("UItemComponent::UItemComponent()"));

	PrimaryComponentTick.bCanEverTick = false;
}

void UItemComponent::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("UItemComponent::BeginPlay()"));

	UActorComponent::BeginPlay();
}

void UItemComponent::Execute(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UStoryComponent::Execute"));
}
