// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObject.generated.h"

UCLASS()
class TPTOBJECTS_API ABaseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseObject();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "BaseObject")
	void SetWidgetVisible(bool Visible);

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class USceneComponent> RootSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class UOutlineComponent> OutlineComp;


	// 가까운 오브젝트 확인용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | NearWidget")
	TObjectPtr<class UWidgetComponent> NearWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | NearWidget")
	TSubclassOf<class UUserWidget> NearWidgetClass;
	
    // AI percrption(적이 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | AI")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject")
	bool bCanInteract = true;

};
