// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TPTLocalPlayerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTLocalPlayerSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()
public:
    // 플레이어 인벤토리 (아이템 ID, 수량)
    UPROPERTY()
    TMap<int32,int32> Inventory;

    UPROPERTY()
    int32 CoreEnergy;

};
