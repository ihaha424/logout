// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.h"
#include "FRandomDT.generated.h"


USTRUCT(BlueprintType)
struct FRandomDT : public FTableRowBase
{
    GENERATED_BODY()

public:
    FRandomDT()
        : ItemType(EItemType::None)
        , RandomProbability(10)
        , GenerateCount(1)
    {}

public:
    // 아이템 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;             

    // 랜덤박스 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RandomProbability;    

    // 물음표 박스에서의 아이템 생성 갯수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GenerateCount = 0; 
};
