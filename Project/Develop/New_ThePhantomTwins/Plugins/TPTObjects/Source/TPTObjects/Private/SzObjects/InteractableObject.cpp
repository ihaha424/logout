// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/InteractableObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"


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

    // 위젯 설정
    if (InteractWidgetComp)
    {
        if (InteractWidgetClass)
        {
            // 위젯 클래스 설정
            InteractWidgetComp->SetWidgetClass(InteractWidgetClass);
        }

        // 일단 숨김 상태로 시작
        InteractWidgetComp->SetVisibility(false);

        // 실제 위젯 인스턴스 가져오기
        if (UUserWidget* CreatedWidget = InteractWidgetComp->GetUserWidgetObject())
        {
            if (UInteractWidget* InteractWidget = Cast<UInteractWidget>(CreatedWidget))
            {
                // ActionTxt가 비어있지 않다면 텍스트 설정
                if (!ActionTxt.IsEmpty())
                {
                    InteractWidget->SetText(ActionTxt);
                }
            }
        }
    }

    // BeginPlay에서 모든 MeshComponent의 OverlayMaterial을 미리 캐싱
    TArray<UMeshComponent*> MeshComponents;
    GetComponents<UMeshComponent>(MeshComponents);

    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            UMaterialInterface* CurrentOverlay = MeshComp->GetOverlayMaterial();
            if (CurrentOverlay)
            {
                // OverlayMaterial이 존재하는 경우에만 TMap에 저장
                CachedOverlayMaterials.Add(MeshComp, CurrentOverlay);
            }
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
    bCanInteract = bIsDetected;

    if (!Interactor->IsLocallyControlled()) return bCanInteract;
    SetWidgetVisible(bCanInteract);

    //UE_LOG(LogTemp, Log,
    //    TEXT("AItemObject::CanInteract - %s | %s | Role: %s"),
    //    bCanInteract ? TEXT("true") : TEXT("false"),
    //    *Interactor->GetName(),
    //    *UEnum::GetValueAsString(GetLocalRole()));

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

    // UDecalComponent 가 있고 이름이 FName("InteractDecal") 이라면 bVisible 상태에 따라 아래 로직 실행
    if (InteractDecalComp)
    {
        InteractDecalComp->SetHiddenInGame(!bVisible);
    }

}

void AInteractableObject::ShowOverlayOutline(bool bVisible)
{
    // 캐시된 MeshComponent들만 순회하면서 활성화/비활성화
    for (auto& Pair : CachedOverlayMaterials)
    {
        UMeshComponent* MeshComp = Pair.Key;
        UMaterialInterface* OverlayMat = Pair.Value;

        if (MeshComp)
        {
            if (bVisible)
            {
                // OverlayMaterial 활성화 (원본 머티리얼로 복원)
                MeshComp->SetOverlayMaterial(OverlayMat);
            }
            else
            {
                // OverlayMaterial 비활성화
                MeshComp->SetOverlayMaterial(nullptr);
            }
        }
    }
}

void AInteractableObject::OnRep_bIsActived()
{
	if (bIsActived)
	{
		ShowOverlayOutline(!bIsActived);
	}
}
