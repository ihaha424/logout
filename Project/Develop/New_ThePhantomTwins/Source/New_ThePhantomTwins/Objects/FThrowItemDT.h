
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

    // 발사 방향과 속도("어느 방향으로, 얼마나 빠르게" 물체를 날릴지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector    LaunchVelocity;      

    /*
        데이터테이블에서 수정하지 않음 (=0.f 그대로 둠)
        → 월드 기본 중력 사용 (예: -980.f)
        
        데이터테이블에 -490.f 입력
        → 월드 -980 대비 절반, GravityScale = 0.5
        
        데이터테이블에 -2000.f 입력
        → 월드 -980 대비 약 2배, GravityScale ≈ 2.04
    */
    // 중력 가속도 (0이면 기본값 : -980.f 으로 들어감)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       OverrideGravityZ;      

    // 충돌 체크할 때 투사체 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float       ProjectileRadius;    
};
