// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/InteractableComponent.h"

// TODO: Delete Debug Library
#include "Kismet/KismetSystemLibrary.h"

//AI Perception
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"


// Sets default values
ABaseObject::ABaseObject()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("OverlapAll"));


    // 위젯 컴포넌트는 항상 생성하지만, 사용 여부에 따라 설정/표시 여부 제어
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ObjectWidget"));
    WidgetComponent->SetupAttachment(RootComponent);
    WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    WidgetComponent->SetDrawSize(FVector2D(10, 10));
    WidgetComponent->SetRelativeLocation(FVector(0, 0, 100));
    WidgetComponent->SetVisibility(false); // 기본은 비활성화

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(RootComponent);
    SphereCollisionComp->ComponentTags.Add(FName("Object"));
	SphereCollisionComp->SetSphereRadius(50.0f);
	SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // NetWork
    bReplicates = true;

    // AI Perception
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    StimuliSource->bAutoRegister = true;

    //StimuliSource에 감지할 감각을 등록
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

    // "Object" 태그 추가
    Tags.Add(FName("Object"));
}

void ABaseObject::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 설정 (필요할 때만)
    if (bUseUI && WidgetComponent)
    {
        if (WidgetClass)
        {
            WidgetComponent->SetWidgetClass(WidgetClass);
            WidgetComponent->SetVisibility(true);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set for %s"), *GetName());
        }
    }
    else
    {
        if (WidgetComponent)
        {
            WidgetComponent->SetVisibility(false);
        }
    }

    // 인터랙션 컴포넌트 확인
    if (!InteractComp)
    {
        InteractComp = FindComponentByClass<UInteractableComponent>();
        if (!InteractComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("No InteractableComponent found on %s"), *GetName());
        }
    }
}

#if WITH_EDITOR
void ABaseObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(ABaseObject, ObjectType))
    {
        bIsInventoryObject =
            (ObjectType == EObjectType::Item ||
                ObjectType == EObjectType::Text ||
                ObjectType == EObjectType::Tool);
    }
}
#endif

void ABaseObject::OnInteractSever_Implementation(APawn* Interactor)
{
    if (InteractComp)
    {
        InteractComp->ExecuteSever(Interactor);

        // 인벤토리 오브젝트라면 픽업 상태 설정
        if (ObjectType == EObjectType::Item || ObjectType == EObjectType::Text || ObjectType == EObjectType::Tool)
        {
            bIsPickedUp = true;
        }

        if (bDestory)
        {
            InteractComp->DeleteLogic();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractComp is nullptr!"));
    }
}

void ABaseObject::OnInteractClient_Implementation(APawn* Interactor)
{
    if (InteractComp)
    {
        InteractComp->ExecuteClient(Interactor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractComp is nullptr!"));
    }
}


bool ABaseObject::CanInteract_Implementation(const APawn* Interactor) const
{
    return bCanInteract;
}

bool ABaseObject::GetPickedUp_Implementation() const
{
    return bIsPickedUp;
}


void ABaseObject::SetWidgetVisibility_Implementation(bool Visible)
{
    WidgetComponent->SetVisibility(Visible);
}