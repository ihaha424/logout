// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HackingGauge.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API UHackingGauge : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void UpdateHoldTime(float Time);

protected:
	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> TimeText;
};
