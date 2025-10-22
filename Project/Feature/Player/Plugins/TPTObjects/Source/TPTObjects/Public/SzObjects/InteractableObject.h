// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Interact.h"
#include "SzUI/InteractWidget.h"
#include "InteractableObject.generated.h"

UCLASS()
class TPTOBJECTS_API AInteractableObject : public ABaseObject, public IInteract
{
	GENERATED_BODY()
	
public:	
	AInteractableObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "InteractableObject")
	virtual void SetWidgetVisible(bool bVisible);

	UFUNCTION(BlueprintCallable)
	virtual void SetActive(bool bIsActive)
	{
		bIsActived = bIsActive;
	}
public:	
	// 가까운 오브젝트 확인용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> InteractWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TSubclassOf<class UInteractWidget> InteractWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	FText ActionTxt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject", ReplicatedUsing = OnRep_bIsActived)
	bool bIsActived = false;

	UFUNCTION()
	virtual void OnRep_bIsActived();
};
