// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Engine/StaticMesh.h"
#include "GameplayTagContainer.h"

#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "ItemData.generated.h"

// 아이템 타입 열거형
UENUM(BlueprintType)
enum class EItemType : uint8
{
    // None
	None,  
    // 폭죽
    FireCracker, 
    // 카메라
    Camera,
    // 체력 팩
    HealPack,
    // 멘탈 팩
    MentalPack,
    // 열쇠
    Key,
    // 에너지 드링크
    EnergyDrink,
    // 오라 탐지기
    AuraDetector,
    // 룰렛
	Roulette
};

// Item DataTable 구조체
USTRUCT(BlueprintType)
struct FItemDataTable : public FTableRowBase
{
    GENERATED_BODY()
public:
    FItemDataTable()
        : ItemType(EItemType::None)
        , ItemIcon(nullptr)
        , ItemMesh(nullptr)
        , GameplayTag()
        , GameAbility(nullptr)
        , GameEffect(nullptr)
        , Description(FText::GetEmpty())
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag GameplayTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayAbility> GameAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> GameEffect;
};
