// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_Puzzle.h"
#include "../P_CM_Log.h"
#include "../DataAsset/P_CM_PuzzleData.h"


void FP_CM_Puzzle::Initialize(const UP_CM_PuzzleData* data)
{
	LogicValue = data->LogicValue;
	EmotionValue = data->EmotionValue;
	Cards.Empty();
	for (auto* CardData : data->CardList)
	{
		FP_CM_Card Card;
		Card.LogicValue = CardData->LogicValue;
		Card.EmotionValue = CardData->EmotionValue;
		Cards.Add(Card);
	}
}

void FP_CM_Puzzle::HandleClick(int32 CardIndex)
{
	UE_LOG(LogCommunityMetaphorPuzzle, Error, TEXT("UP_CM_WidgetCard: NativeOnMouseButtonDown: %d"), CardIndex)
	if (Cards.IsValidIndex(CardIndex))
	{
		Cards[CardIndex].IsToggle = !Cards[CardIndex].IsToggle;
		if (Cards[CardIndex].IsToggle)
		{
			UE_LOG(LogCommunityMetaphorPuzzle, Error, TEXT("UP_CM_WidgetCard: toggle"));

			CurLogicValue += Cards[CardIndex].LogicValue;
			CurEmotionValue += Cards[CardIndex].EmotionValue;
		}
		else
		{
			UE_LOG(LogCommunityMetaphorPuzzle, Error, TEXT("UP_CM_WidgetCard: UNtoggle"));

			CurLogicValue -= Cards[CardIndex].LogicValue;
			CurEmotionValue -= Cards[CardIndex].EmotionValue;
		}
	}
}

TArray<FP_CM_Card> FP_CM_Puzzle::GetCards() const
{
	return Cards;
}

bool FP_CM_Puzzle::CheckPuzzle()
{
	if (CurLogicValue >= LogicValue && CurEmotionValue >= EmotionValue)
		return true;
	return false;
}

int32 FP_CM_Puzzle::GetLogicValue() const
{
	return CurLogicValue;
}

int32 FP_CM_Puzzle::GetEmotionValue() const
{
	return CurEmotionValue;
}

