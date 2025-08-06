// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Log/TPTLog.h"

AConsoleObject::AConsoleObject() : AInteractableObject()
{

}

void AConsoleObject::BeginPlay()
{
    Super::BeginPlay();

}

void AConsoleObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

bool AConsoleObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{

}

void AConsoleObject::OnRep_bIsActived()
{

}

void AConsoleObject::OnInteractClient_Implementation(const APawn* Interactor)
{

}

void AConsoleObject::OnInteractServer_Implementation(const APawn* Interactor)
{

}

void AConsoleObject::SetWidgetVisible(bool bVisible)
{

}
