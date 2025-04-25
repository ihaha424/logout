// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "CabinetObject.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTTEST0424_API ACabinetObject : public AObjectBase
{
	GENERATED_BODY()
	
public:
    ACabinetObject();

public:
	
    // 상호작용 영역 컴포넌트 (트리거)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USphereComponent> InteractionTrigger;
    
    // 메쉬 컴포넌트 (시각적 표현)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> MeshComponent;
    
    // 상호작용 컴포넌트 (기능)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UInteractiveComponent> InteractComp;
    

protected:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                         bool bFromSweep, const FHitResult& SweepResult);

};
