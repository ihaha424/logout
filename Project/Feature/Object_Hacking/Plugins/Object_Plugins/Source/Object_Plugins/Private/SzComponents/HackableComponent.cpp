// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/HackableComponent.h"

// Sets default values for this component's properties
UHackableComponent::UHackableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UHackableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHackableComponent::Execute()
{
	UE_LOG(LogTemp, Log, TEXT("UHackableComponent::Execute"));
}
