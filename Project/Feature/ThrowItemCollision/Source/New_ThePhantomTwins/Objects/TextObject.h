// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "TextObject.generated.h"


UCLASS()
class NEW_THEPHANTOMTWINS_API ATextObject : public AInteractableObject
{
	GENERATED_BODY()

public:
	ATextObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	//virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	//virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual void OnRep_bIsActived() override;

	UFUNCTION(BlueprintNativeEvent, Category = "TextObject")
    void SetData2Widget();	// data를 위젯으로 세팅(자식클래스든, BP에서든 무조건 구현해줘야함)
	void SetData2Widget_Implementation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextObject")
	TSubclassOf<class UUserWidget> AttachWidgetClass;
};
