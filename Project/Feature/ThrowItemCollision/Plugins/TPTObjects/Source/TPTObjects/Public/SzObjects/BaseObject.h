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

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class USceneComponent> RootSceneComp;

	// Interact위한 collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject | Components")
	TObjectPtr<class UOutlineComponent> OutlineComp;

	// Decal
	UPROPERTY(BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class UDecalComponent> InteractDecalComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseObject")
	bool bCanInteract = true;
};
