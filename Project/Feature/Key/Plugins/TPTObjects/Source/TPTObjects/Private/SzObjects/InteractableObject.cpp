// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/InteractableObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"


AInteractableObject::AInteractableObject() : ABaseObject()
{
    bReplicates = true;

    // 위젯 컴포넌트는 항상 생성하지만, 사용 여부에 따라 설정/표시 여부 제어
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearObjectWidget"));
	InteractWidgetComp->SetupAttachment(RootSceneComp);
	InteractWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidgetComp->SetDrawSize(FVector2D(10, 10));
	InteractWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	InteractWidgetComp->SetVisibility(false); // 기본은 비활성화
}

void AInteractableObject::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 설정 (필요할 때만)
    if (InteractWidgetComp)
    {
        if (InteractWidgetClass)
        {
            InteractWidgetComp->SetWidgetClass(InteractWidgetClass);
            InteractWidgetComp->SetVisibility(false);
        }
    }
}

void AInteractableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AInteractableObject, bIsActived);
}

bool AInteractableObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
    if (!Interactor->IsLocallyControlled()) return false;

    bCanInteract = bIsDetected;

    //UE_LOG(LogTemp, Log,
    //    TEXT("AItemObject::CanInteract - %s | %s | Role: %s"),
    //    bCanInteract ? TEXT("true") : TEXT("false"),
    //    *Interactor->GetName(),
    //    *UEnum::GetValueAsString(GetLocalRole()));

    SetWidgetVisible(bCanInteract);

    return bCanInteract;
}

void AInteractableObject::OnInteractServer_Implementation(const APawn* Interactor)
{

}

void AInteractableObject::OnInteractClient_Implementation(const APawn* Interactor)
{
    SetWidgetVisible(false);
}

void AInteractableObject::SetWidgetVisible(bool bVisible)
{
    InteractWidgetComp->SetVisibility(bVisible);
}

void AInteractableObject::OnRep_bIsActived()
{

}
