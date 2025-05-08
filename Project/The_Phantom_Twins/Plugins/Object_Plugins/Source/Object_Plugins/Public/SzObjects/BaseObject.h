// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SzComponents/Interaction.h"
#include "BaseObject.generated.h"

UCLASS()
class OBJECT_PLUGINS_API ABaseObject : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	ABaseObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteract_Implementation(APawn* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bDestory = false;

};
