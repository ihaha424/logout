// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataFragmentPickupWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UDataFragmentPickupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetText(const FText& Text);

protected:
	// 바인딩된 텍스트 위젯
    UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextWidget;
};
