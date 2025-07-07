// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P_CM_WidgetBoard.generated.h"

class UP_CM_PuzzleData;
class UVerticalBox;
class UUniformGridPanel;
class UTextBlock;
class UP_CM_WidgetComment;
class UP_CM_WidgetCard;
class AP_CM_PuzzleActor;

UCLASS()
class THE_PHANTOM_TWINS_API UP_CM_WidgetBoard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetOwner(AP_CM_PuzzleActor* Owner);
	bool InitializeData(const UP_CM_PuzzleData* Data);

	UFUNCTION(BlueprintCallable)
	void UpdateUIFromState(const FPuzzleState& State);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TSubclassOf<UP_CM_WidgetComment> CommentBoxClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TSubclassOf<UP_CM_WidgetCard> CardBoxClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Community Metaphor Puzzle")
	TObjectPtr<AP_CM_PuzzleActor> Owner;
private:
	bool InitialComment(const UP_CM_PuzzleData* Data);
	bool InitialCard(const UP_CM_PuzzleData* Data);
	bool InitialPuzzleData(const UP_CM_PuzzleData* Data);

	/**
	 * @brief : Change To Value Text
			Localize (multilingual support)	: ✅ Fully supported (automatically translated in case of language change)
			Runtime Transformation			: Using FText::AsNumber()** Number Formatting (comma, decimal, etc.)** Automatic processing according to degree culture
			Editor extraction target		: NSLOCTEXT(...) is automatically detected by the localize extractor
			Cost							: Slightly heavy but required if localizing is required
	 * @param Block		: TextBlock
	 * @param First		: Denominator
	 * @param Second	: Numerator
	 */
	void SetValueFormat(UTextBlock* Block, int32 First, int32 Second);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CommentBox;
	TArray<UP_CM_WidgetComment*> CommentBoxList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> CardBox;
	TArray<UP_CM_WidgetCard*> CardBoxList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LogicalValue;
	int32 TotalLogicalValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EmotionValue;
	int32 TotalEmotionValue;
};


