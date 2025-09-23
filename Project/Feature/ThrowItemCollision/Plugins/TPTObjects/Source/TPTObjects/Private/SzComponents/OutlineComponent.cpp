// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/OutlineComponent.h"
#include "Components/MeshComponent.h"

UOutlineComponent::UOutlineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UOutlineComponent::BeginPlay()
{
	Super::BeginPlay();
    AActor* Owner = GetOwner();
    if (!Owner) return;

    TargetMesh = Owner->FindComponentByClass<UMeshComponent>();

    if (TargetMesh && OverlayMaterial)
    {
        OverlayMID = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
        OverlayMID->SetVectorParameterValue("OutlineColor", OutlineColor);
        OverlayMID->SetScalarParameterValue("LineScale", LineScale);
        TargetMesh->SetOverlayMaterialMaxDrawDistance(MaxDrawDistance);
    }
}


void UOutlineComponent::SetOutline(bool bActive)
{
    if (!TargetMesh) return;

    if (bActive)
    {
        TargetMesh->SetOverlayMaterial(OverlayMID);
    }
    else
    {
        TargetMesh->SetOverlayMaterial(nullptr);
    }
}
