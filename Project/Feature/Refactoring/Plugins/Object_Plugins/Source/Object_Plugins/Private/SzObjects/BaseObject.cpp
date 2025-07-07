// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/OutlineComponent.h"

//AI Perception
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"


// Sets default values
ABaseObject::ABaseObject()
{
    PrimaryActorTick.bCanEverTick = false;

    // NetWork
    bReplicates = true; // 자식들이 각각 하는게 좋을 것 같다?????????????????????

    // Root Scene
    RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("MeshComponent"));
    SetRootComponent(RootSceneComp);

    // Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(MeshComp);
    SphereCollisionComp->SetSphereRadius(50.0f);
    SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // Outline
    OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

    // 위젯 컴포넌트는 항상 생성하지만, 사용 여부에 따라 설정/표시 여부 제어
    NearWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearObjectWidget"));
    NearWidgetComp->SetupAttachment(MeshComp);
    NearWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    NearWidgetComp->SetDrawSize(FVector2D(10, 10));
    NearWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
    NearWidgetComp->SetVisibility(false); // 기본은 비활성화

    // AI Perception
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    StimuliSource->bAutoRegister = true;
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

    // "Interactable" 태그 추가
    Tags.Add(FName("Interactable"));
}

void ABaseObject::BeginPlay()
{
    Super::BeginPlay();

    // Near Object Widget
    if (NearWidgetComp)
    {
        if (NearWidgetClass)
        {
            NearWidgetComp->SetWidgetClass(NearWidgetClass);
            NearWidgetComp->SetVisibility(false);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set for %s"), *GetName());
        }
    }

    // Outline
    if (OutlineComp)
    {
        OutlineComp->SetOutline(false);
    }
}

void ABaseObject::SetWidgetVisible(bool Visible)
{
    NearWidgetComp->SetVisibility(Visible);
}