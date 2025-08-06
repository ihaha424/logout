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

	UFUNCTION(BlueprintCallable, Category = "InteractableObject")
	virtual void SetWidgetVisible(bool bVisible);

public:	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject", ReplicatedUsing = OnRep_bIsActived)
	bool bIsActived = false;

	UFUNCTION()
	virtual void OnRep_bIsActived();
};
