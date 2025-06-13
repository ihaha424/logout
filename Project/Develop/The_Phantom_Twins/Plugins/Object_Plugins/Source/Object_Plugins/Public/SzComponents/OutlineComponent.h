// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "OutlineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UOutlineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UOutlineComponent();

protected:
    virtual void BeginPlay() override;

public:
    void SetOutline(bool bActive);

    UPROPERTY(EditDefaultsOnly, Category="Outline")
    TObjectPtr<UMaterialInterface> OverlayMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")
    FLinearColor OutlineColor = FLinearColor(0,0,3,1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")
    float LineScale = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Outline")
    float MaxDrawDistance = 5000.0f;

private:
    TObjectPtr<UMaterialInstanceDynamic> OverlayMID;
    TObjectPtr<UMeshComponent> TargetMesh;
};
