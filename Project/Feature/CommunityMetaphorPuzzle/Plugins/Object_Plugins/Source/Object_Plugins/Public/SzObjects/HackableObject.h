// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SzInterface/Hacking.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "HackableObject.generated.h"

UCLASS()
class OBJECT_PLUGINS_API AHackableObject : public APawn, public IHacking
{
	GENERATED_BODY()
	
public:	
	AHackableObject();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 해킹 실행 (E키 홀딩)
	virtual void OnHackingStartedServer_Implementation(APawn* Interactor) override;
	virtual void OnHackingStartedClient_Implementation(APawn* Interactor) override;

	// 해킹 완료 후 로직 (CCTV 보임, 적 무력화 등)
	virtual void OnHackingCompletedServer_Implementation(APawn* Interactor) override;
	virtual void OnHackingCompletedClient_Implementation(APawn* Interactor) override;
    
	// 해킹 가능 여부 체크 (false => 해킹 전 / true => 해킹 완료)
	virtual bool CanBeHacked_Implementation() const override;

	// 해킹 초기화(해킹 안 된 상태로 만들기)
	virtual void ClearHacking_Implementation() override;

	virtual void SetWidgetVisibility_Implementation(bool Visible) override;

	void SetOutline(bool bActive);

protected:
	// 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> WidgetComponent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	TObjectPtr<class UHackableComponent> HackingComp;

	// Outline
    UPROPERTY(EditDefaultsOnly, Category="Outline")
    TObjectPtr<UMaterialInterface> OverlayMaterial;	// Overlay 머티리얼 레퍼런스

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")	
    FLinearColor OutlineColor = FLinearColor(3,0,0,1);	// OutlineColor 파라미터
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")
    float LineScale = 5.0f;	// LineScale 파라미터

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")
	float MaxDrawDistance = 2000.0f;	// 최대 드로우 거리 (이 거리 이상에서는 오버레이가 보이지 않음)

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OverlayMID;	// 동적 머티리얼 인스턴스 (머티리얼 인스턴스가 필요할 때마다 생성됨)
	
	UPROPERTY()
	TObjectPtr<APawn> CurrentHackingPawn;	// 현재 해킹 중인 플레이어를 추적하기 위한 변수
};
