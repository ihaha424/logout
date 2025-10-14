// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/CharacterType.h"
#include "Data/MapType.h"
#include "TPTSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FDoorState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsOpened;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDestroyed;
};

UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FIdentifyCharacterData IdentifyCharacterData;
	UPROPERTY()
	FIdentifyMapData IdentifyMapData;
	// 데이터 조각을 획득한 플레이어의 위치
	UPROPERTY()
	FVector PlayerLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator PlayerRotation = FRotator::ZeroRotator;

	UPROPERTY()
	int32 DataFragmentNum;
	// 부서지는 오브젝트 상태 (오브젝트 ID → 숨김 여부)
	UPROPERTY()
	TMap<int32, bool> DestructibleObjectStates;
	// 문 상태 (문 ID → 잠김(true)/ 잠금 해제(false))
	UPROPERTY()
	TMap<int32, FDoorState> DoorStates;
	// 아이템 박스 상태 (아이템박스 ID → 열림 여부)
	UPROPERTY()
	TMap<int32, bool> ItemBoxStates;
	// 맵 내 아이템 획득 상태 (아이템 ID → 획득 여부)
	UPROPERTY()
	TMap<int32, bool> ItemPickupStates; 
};
