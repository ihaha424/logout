// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/StaticObject.h"
#include "Net/UnrealNetwork.h"

AStaticObject::AStaticObject() : ABaseObject()
{
	bReplicates = true;
}

void AStaticObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStaticObject, isActived);
}
