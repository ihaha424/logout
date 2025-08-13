// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Objects/ItemData.h"
#include "ItemToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UItemToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintCallable)
    void SetItemName(EItemType eItemType);

	UFUNCTION(BlueprintCallable)
    void SetItemDescription(EItemType eItemType);

protected:
	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> ItemName;

	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> ItemDescription;
};
