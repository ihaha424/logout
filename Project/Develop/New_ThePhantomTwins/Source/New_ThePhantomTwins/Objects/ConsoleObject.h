// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "ConsoleObject.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AConsoleObject : public AInteractableObject
{
	GENERATED_BODY()
	
public:	
	AConsoleObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual void SetWidgetVisible(bool bVisible) override;

	virtual void OnRep_bIsActived() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

};
