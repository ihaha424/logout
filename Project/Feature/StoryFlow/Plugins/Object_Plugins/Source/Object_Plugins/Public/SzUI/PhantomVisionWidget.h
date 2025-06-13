// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhantomVisionWidget.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API UPhantomVisionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "CCTV")
    void SetCCTVIDTxt(int32 CurrentCCTVID);

protected:
	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> CCTVIDTxt;
};
