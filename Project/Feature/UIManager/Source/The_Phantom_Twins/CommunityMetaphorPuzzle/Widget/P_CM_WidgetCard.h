// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P_CM_WidgetBase.h"
#include "P_CM_WidgetCard.generated.h"

class FP_CM_Card;
class AP_CM_PuzzleActor;
class UP_CM_CardData;
class UTextBlock;

UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_WidgetCard : public UP_CM_WidgetBase
{
	GENERATED_BODY()

public:
	bool InitialData(const UP_CM_CardData* Data, int32 CardIndexInput);
	virtual void NativeConstruct() override;
	void SetPuzzle(AP_CM_PuzzleActor* PuzzleActor);


	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void OnClicked();
	virtual void Effect_Implementation(bool IsToggle) override;

public:
	int32 CardIndex;

private:
	TObjectPtr<AP_CM_PuzzleActor> PuzzleOwner;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LogicalValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EmotionValue;

	FText TextValueData;
	FText LogicalValueData;
	FText EmotionValueData;
};
