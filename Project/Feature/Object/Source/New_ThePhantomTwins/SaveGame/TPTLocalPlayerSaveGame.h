// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TPTLocalPlayerSaveGame.generated.h"

struct FItemSlot;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTLocalPlayerSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()
public:
    // 플레이어 인벤토리 (아이템 ID, 수량)
    UPROPERTY(BlueprintReadWrite)
    TArray<FItemSlot> InventorySlots;

    UPROPERTY(BlueprintReadWrite)
    int32 CoreEnergy;
};
