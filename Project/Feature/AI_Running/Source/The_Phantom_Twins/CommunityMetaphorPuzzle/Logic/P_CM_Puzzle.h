// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P_CM_Card.h"

class UP_CM_PuzzleData;

class THE_PHANTOM_TWINS_API FP_CM_Puzzle
{
public:
    void Initialize(const UP_CM_PuzzleData* data);
    void HandleClick(int32 CardIndex);
    TArray<FP_CM_Card> GetCards() const;
    

    /**
     * @brief   : Check the Success or Fail Puzzle
                  Call by Player UI Event
     * @return  :Success or Fail Puzzle
     */
    bool CheckPuzzle();

    int32 GetLogicValue() const;
    int32 GetEmotionValue() const;

private:
    int32 LogicValue;
    int32 EmotionValue;
    TArray<FP_CM_Card> Cards;

    int32 CurLogicValue;
    int32 CurEmotionValue;
};

