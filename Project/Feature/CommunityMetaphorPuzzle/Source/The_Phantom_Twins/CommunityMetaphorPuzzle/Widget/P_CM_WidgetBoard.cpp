// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_WidgetBoard.h"
#include "../P_CM_Log.h"
#include "Components/VerticalBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TextBlock.h"
#include "P_CM_WidgetCard.h"
#include "P_CM_WidgetComment.h"
#include "../DataAsset/P_CM_PuzzleData.h"
#include "../P_CM_PuzzleState.h"
#include "../P_CM_PuzzleActor.h"


void UP_CM_WidgetBoard::SetOwner(AP_CM_PuzzleActor* OwnerActor)
{
	Owner = OwnerActor;
}

bool UP_CM_WidgetBoard::InitializeData(const UP_CM_PuzzleData* Data)
{
	if (!Data)
		return false;

	if (!InitialComment(Data))
	{
		UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Community Metaphor Puzzle Initialize Fail.\
			Please Check the P_CM_PuzzleActor: UP_CM_PuzzleData: CommnetList."));
		return false;
	}

	if (!InitialCard(Data))
	{
		UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Community Metaphor Puzzle Initialize Fail.\
			Please Check the P_CM_PuzzleActor: UP_CM_PuzzleData: CardList."));
		return false;
	}

	if (!InitialPuzzleData(Data))
	{
		UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Community Metaphor Puzzle Initialize Fail.\
			Please Check the P_CM_PuzzleActor: UP_CM_PuzzleData: CardList."));
		return false;
	}

	return true;
}

void UP_CM_WidgetBoard::NativeConstruct()
{
	Super::NativeConstruct();
	Owner->InitializePuzzle(Owner->InitializeData);
}

void UP_CM_WidgetBoard::UpdateUIFromState(const FPuzzleState& State)
{
	for (int index = 0; index < State.bCardFlipped.Num(); index++)
	{
		CardBoxList[index]->Effect(State.bCardFlipped[index]);
	}

	SetValueFormat(LogicalValue, State.LogicalValue, TotalLogicalValue);
	SetValueFormat(EmotionValue, State.EmotionValue, TotalEmotionValue);
}

bool UP_CM_WidgetBoard::InitialComment(const UP_CM_PuzzleData* Data)
{
	if (!CommentBox)
		return false;

	TArray<UP_CM_CommentData*> CommentList = Data->CommentList;
	CommentBoxList.SetNum(CommentList.Num());

	int32 index = 0;
	for (const UP_CM_CommentData* Comment : CommentList)
	{
		if (!Comment)
		{
			return false;
		}
		UP_CM_WidgetComment* NewComment = CreateWidget<UP_CM_WidgetComment>(this, CommentBoxClass);
		if (NewComment)
		{
			NewComment->InitialData(Comment);
			CommentBox->AddChildToVerticalBox(NewComment);
			CommentBoxList[index] = NewComment;
			index++;
		}
	}

	return true;
}

bool UP_CM_WidgetBoard::InitialCard(const UP_CM_PuzzleData* Data)
{
	if (!CardBox)
		return false;

	TArray<UP_CM_CardData*> CardList = Data->CardList;
	CardBoxList.SetNum(Data->CardList.Num());

	int32 index = 0;
	for (const UP_CM_CardData* Card : CardList)
	{
		UP_CM_WidgetCard* NewCard = CreateWidget<UP_CM_WidgetCard>(this, CardBoxClass);
		if (NewCard)
		{
			if (!NewCard)
			{
				return false;
			}
			NewCard->InitialData(Card, index);
			NewCard->SetPuzzle(Owner);
			UUniformGridSlot* GridSlot = CardBox->AddChildToUniformGrid(NewCard, 0, index);
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
			CardBoxList[index] = NewCard;
			index++;
		}
	}

	return true;
}

bool UP_CM_WidgetBoard::InitialPuzzleData(const UP_CM_PuzzleData* Data)
{
	if (!LogicalValue || !EmotionValue)
		return false;

	TotalLogicalValue = Data->LogicValue;
	TotalEmotionValue = Data->EmotionValue;
	SetValueFormat(LogicalValue, 0, TotalLogicalValue);
	SetValueFormat(EmotionValue, 0, TotalEmotionValue);
	return false;
}

void UP_CM_WidgetBoard::SetValueFormat(UTextBlock* Block, int32 First, int32 Second)
{
	FText DisplayText;

	DisplayText = FText::Format(
		NSLOCTEXT("MyNamespace", "KOverN", "{0} / {1}"),
		FText::AsNumber(First),
		FText::AsNumber(Second));
	Block->SetText(DisplayText);
}
