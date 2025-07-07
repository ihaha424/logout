// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/CardKey.h"
#include "SzComponents/ItemComponent.h"

ACardKey::ACardKey()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractComp = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	//InteractComp->SetupAttachment(RootComponent);
	ObjectType = EObjectType::Item;
	bDestory = true;
}
