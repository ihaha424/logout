// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/ItemComponent.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
}

void UItemComponent::ExecuteSever(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UItemComponent::Execute"));
}
