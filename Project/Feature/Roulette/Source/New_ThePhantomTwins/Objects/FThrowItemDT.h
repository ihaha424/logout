// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.h"
#include "FThrowItemDT.generated.h"


USTRUCT(BlueprintType)
struct FThrowItemDT : public FTableRowBase
{
    GENERATED_BODY()

public:
    FThrowItemDT(){}


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;             // 아이템 타입

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    LaunchVelocity;      // 발사 방향과 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       ProjectileRadius;      // 충돌 체크할 때 투사체 반경

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       OverrideGravityZ;      // 중력 가속도
};
