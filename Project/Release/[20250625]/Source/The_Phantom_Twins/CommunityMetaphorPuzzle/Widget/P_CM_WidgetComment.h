// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P_CM_WidgetBase.h"
#include "P_CM_WidgetComment.generated.h"

class UP_CM_CommentData;
class UImage;
class UTextBlock;

UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_WidgetComment : public UP_CM_WidgetBase
{
	GENERATED_BODY()
	
public:
	bool InitialData(const UP_CM_CommentData* Data);
	virtual void NativeConstruct() override;


	virtual void Effect_Implementation(bool IsToggle) override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text;

	TObjectPtr<UTexture2D> IconData;
	FText TextData;
};
