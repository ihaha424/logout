// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Net/UnrealNetwork.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SzComponents/OutlineComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

//AI Perception
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"

ABaseObject::ABaseObject()
{
	PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    // Root Scene
    RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComp);

    // Mesh
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComp->SetupAttachment(RootSceneComp);
    //MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(RootSceneComp);
    SphereCollisionComp->SetSphereRadius(50.0f);
    SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // Outline
    OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

    // 위젯 컴포넌트는 항상 생성하지만, 사용 여부에 따라 설정/표시 여부 제어
    NearWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearObjectWidget"));
    NearWidgetComp->SetupAttachment(RootSceneComp);
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

    // 위젯 설정 (필요할 때만)
    if (NearWidgetComp)
    {
        if (NearWidgetClass)
        {
            NearWidgetComp->SetWidgetClass(NearWidgetClass);
            NearWidgetComp->SetVisibility(false);
        }
    }
	
}

void ABaseObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABaseObject, bCanInteract);
}

void ABaseObject::SetWidgetVisible(bool Visible)
{
    NearWidgetComp->SetVisibility(Visible);
}


void ABaseObject::OnRep_SetWidgetVisible(bool Visible)
{
    UE_LOG(LogTemp, Log, TEXT("OnRep_SetWidgetVisible | %s | Role: %s"),
        *GetName(),
        *UEnum::GetValueAsString(GetLocalRole()));

    NearWidgetComp->SetVisibility(Visible);
}
