// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "BaseObject.generated.h"

UCLASS()
class OBJECT_PLUGINS_API ABaseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseObject();

protected:
	virtual void BeginPlay() override;

protected:
	void SetWidgetVisible(bool Visible);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class USceneComponent> RootSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class UOutlineComponent> OutlineComp;


	// 가까운 오브젝트 확인용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | NearWidget")
	TObjectPtr<class UWidgetComponent> NearWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | NearWidget")
	TSubclassOf<class UUserWidget> NearWidgetClass;
	
    // AI percrption(적이 사용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseObject | AI")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseObject")
	bool bCanInteract = true;		// 필요없을수도 잇다???????????????
};