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
    // 소음폭탄
    NoiseBomb,
    // EMP폭탄
    EMP,
    // 체력 팩
    HealPack,
    // 멘탈 팩
    MentalPack,
    // 열쇠
    Key,
    // 네비게이션
    Navigation,
    // 오라 탐지기
    AuraDetector,
    // 물음표박스
    QuestionBox,
    // 꽝
    Miss,
    // 코어에너지
    CoreEnergy
};

// Item DataTable 구조체
USTRUCT(BlueprintType)
struct FItemDataTable : public FTableRowBase
{
    GENERATED_BODY()
public:
    FItemDataTable()
        : ItemType(EItemType::None)
		, ItemName(FText::GetEmpty())
        , Description(FText::GetEmpty())
        , ItemIcon(nullptr)
        , ItemMesh(nullptr)
        , GameplayTag()
        , GameAbility(nullptr)
        , GameEffect(nullptr)
        , ItemSound(nullptr)
        , MaxStack(3)
    {}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType;

    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> ItemSound;

    // 인벤토리 최대 스택
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStack;             
};
