// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SoloEndingPortal.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ASoloEndingPortal : public AInteractableObject
{
	GENERATED_BODY()
	
public:
	ASoloEndingPortal();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

public:
	UFUNCTION(BlueprintCallable, Category = "SoloEndingPortal")
	void ShowSoloPortalActor(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "SoloEndingPortal")
	void SetSoloPortalCollision(bool bActived);

	protected:
    // 서버가 허용한 '상호작용 허용자' (복제하여 클라이언트에서 로컬 판별에 사용)
    UPROPERTY(ReplicatedUsing = OnRep_AllowedInteractor)
    APawn* AllowedInteractor = nullptr;

    UFUNCTION()
    void OnRep_AllowedInteractor();

	UFUNCTION(BlueprintCallable, Category = "SoloEndingPortal")
	void SetAllowedInteractor(APawn* Interactor)
	{
		AllowedInteractor = Interactor;
	}
};
