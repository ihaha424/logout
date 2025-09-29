// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "GameplayTagContainer.h"
#include "BoxObject.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ABoxObject : public AInteractableObject
{
	GENERATED_BODY()
	
public:
	ABoxObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	void ApplyEffectToTarget(const APawn* Interactor);
	void InvokeGameplayCue(const APawn* Interactor);
	void ExecuteTrapBoxGA(const APawn* Interactor);

protected:
	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS", Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditAnywhere, Category = "BoxObject")
	bool bisTrapBox;

	UPROPERTY(EditAnywhere, Category = "BoxObject | UI")
	TSubclassOf<AActor> WarningClass;

	// Child Actor Component에서 Warning Actor를 찾기 위한 함수
	AActor* FindWarningActor();
};