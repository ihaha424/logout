// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "SzComponents/OutlineComponent.h"
#include "Components/BoxComponent.h"


ABaseObject::ABaseObject()
{
	PrimaryActorTick.bCanEverTick = false;

    // Root Scene
    RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComp);

    // Outline
    OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

    // Box
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(RootComponent);
    //BoxComp->SetCollisionProfileName(TEXT("Interactable"));
    BoxComp->SetCollisionProfileName(TEXT("OverlapAll"));
    BoxComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // "Interactable" 태그 추가
    Tags.Add(FName("Interactable"));
}

void ABaseObject::BeginPlay()
{
	Super::BeginPlay();

    // StaticMesh를 찾아서 collision의 CanEverAffectNavigation를 해제
    TArray<UStaticMeshComponent*> MeshComponents;
    GetComponents<UStaticMeshComponent>(MeshComponents);

    for (UStaticMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetCanEverAffectNavigation(false);
        }
    }


    TArray<UDecalComponent*> InteractDecalComponent;
    GetComponents<UDecalComponent>(InteractDecalComponent);

    // BP에서 배치한 UDecalComponent를 찾음
    TArray<UDecalComponent*> DecalComponents;
    GetComponents<UDecalComponent>(DecalComponents);

    for (UDecalComponent* DecalComp : DecalComponents)
    {
        if (DecalComp && DecalComp->GetFName() == FName("InteractDecal"))
        {
            InteractDecalComp = DecalComp;
            InteractDecalComp->SetHiddenInGame(true); // 시작 시 숨김
            break;
        }
    }
}
