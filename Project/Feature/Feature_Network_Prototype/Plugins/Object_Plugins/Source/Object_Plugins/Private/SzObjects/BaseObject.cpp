// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/BaseObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/InteractableComponent.h"

// TODO: Delete Debug Library
#include "Kismet/KismetSystemLibrary.h"



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

    // NetWork
    bReplicates = true;
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

void ABaseObject::OnInteract_Implementation(APawn* Interactor)
{
    if (InteractComp)
    {
        InteractComp->Execute(Interactor);

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
