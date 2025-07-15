// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SzInterface/Interact.h"
#include "ItemObject.generated.h"

UCLASS()
class TPTOBJECTS_API AItemObject : public AInteractableObject
{
	GENERATED_BODY()
	
public:
	AItemObject();

protected:
    virtual void BeginPlay() override;

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
    virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

    // 자식클래스 혹은 블루프린트에서 동작을 구현할 수 있도록 선언(서버용)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemObject")
	void UseItemEffectServer();
	virtual void UseItemEffectServer_Implementation();

    // (Client용)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemObject")
	void UseItemEffectClient();
	virtual void UseItemEffectClient_Implementation();
};
