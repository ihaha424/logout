// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"

AItemObject::AItemObject() : ABaseObject()
{
}

void AItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
