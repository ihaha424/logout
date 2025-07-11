// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/SceneComponent.h"
#include "SzComponents/OutlineComponent.h"

ABaseObject::ABaseObject()
{
	PrimaryActorTick.bCanEverTick = false;

    // Root Scene
    RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComp);

    // Outline
    OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

    // "Interactable" 태그 추가
    Tags.Add(FName("Interactable"));
}

//void ABaseObject::BeginPlay()
//{
//	Super::BeginPlay();
//}
