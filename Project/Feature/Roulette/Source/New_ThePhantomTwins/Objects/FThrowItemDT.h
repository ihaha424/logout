
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
    FThrowItemDT() :
        ItemType(EItemType::None),
        StartOffset({0,0,0}),
        ThrowDistance(1000.0f),
        LaunchVelocity({0,0,0}),
        OverrideGravityZ(0.0f),
        ProjectileRadius(1000.0f)
    {}


public:
    // 아이템 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;             

    // 시작위치에서의 offset
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    StartOffset; 

    // 직선 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float   ThrowDistance;

    // 발사 방향과 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    LaunchVelocity;      

    // 중력 가속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       OverrideGravityZ;      

    // 충돌 체크할 때 투사체 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       ProjectileRadius;    
};
