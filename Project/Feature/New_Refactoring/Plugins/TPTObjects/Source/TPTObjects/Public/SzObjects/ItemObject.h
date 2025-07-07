// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "Engine/DataTable.h"
#include "ItemObject.generated.h"

// 아이템 타입 열거형
UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable  UMETA(DisplayName = "Consumable"),
    Holdable   UMETA(DisplayName = "Inventory Item"),
    None        UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct TPTOBJECTS_API FInventoryItem : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    FInventoryItem()
    {
        ID = 0;
        Name = FString("");
        Description = FString("");
        ItemType = EItemType::None;
        ItemIcon = nullptr;
        ItemMesh = nullptr;
    }

    // 아이템 고유 ID
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (DisplayName = "ID"))
    int32 ID;

    // 아이템 이름
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (DisplayName = "Name"))
    FString Name;

    // 아이템 설명
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (MultiLine = "true", DisplayName = "Description"))
    FString Description;

    // 아이템 타입
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, SaveGame, meta = (DisplayName = "ItemType"))
    EItemType ItemType;

    // 아이템 아이콘 (UI용)
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (DisplayName = "ItemIcon"))
    UTexture2D* ItemIcon;

    // 아이템 메시 (월드 배치용)
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (DisplayName = "ItemMesh"))
    UStaticMesh* ItemMesh;
};


UCLASS()
class TPTOBJECTS_API AItemObject : public ABaseObject
{
	GENERATED_BODY()
	
public:
	AItemObject();

};
