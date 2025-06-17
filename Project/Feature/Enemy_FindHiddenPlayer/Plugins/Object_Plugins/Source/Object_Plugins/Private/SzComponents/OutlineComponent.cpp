// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/OutlineComponent.h"
#include "Components/MeshComponent.h"

// Sets default values for this component's properties
UOutlineComponent::UOutlineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    // Outline
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
        TEXT("/Game/Project_TPT/Assets/Materials/M_Outline.M_Outline"));

    if (MaterialFinder.Succeeded())
    {
        OverlayMaterial = MaterialFinder.Object;
    }

}

void UOutlineComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // MeshComponent 자동 탐색(혹은 수동 지정)
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
