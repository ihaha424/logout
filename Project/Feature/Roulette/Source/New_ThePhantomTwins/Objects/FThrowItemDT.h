
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
    // 아이템 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;             

    // 시작위치에서의 offset
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    StartOffset; 

    // 발사 방향과 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    LaunchVelocity;      

    // 충돌 체크할 때 투사체 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       ProjectileRadius;      

    // 중력 가속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       OverrideGravityZ;      
};
