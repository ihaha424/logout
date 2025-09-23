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

	// 아이템을 먹었을 때 먹은 대상에게 게임플레이 이펙트를 적용시켜주는 함수
	void ApplyEffectToTarget(const APawn* Interactor);	

	// 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
	void InvokeGameplayCue(const APawn* Interactor);		

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;
};
