// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "CCTV.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API ACCTV : public ABaseObject
{
	GENERATED_BODY()
	
public:
	ACCTV();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteractSever_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) const override;

public:
	// CCTV 등급 키
    UPROPERTY(EditAnywhere, Category = "CCTV")
	TObjectPtr<AActor> RequiredKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool bHasKey = false;

	// 카메라 컴포넌트 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<class UCameraComponent> CameraComp;

	// 카메라 전환을 위한 추가 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	bool bIsInCCTVView = false;

	// 이전 뷰 타겟 저장
	UPROPERTY()
	AActor* PreviousViewTarget;
};
