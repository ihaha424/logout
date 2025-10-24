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
	bool bIsUnLocked;

	UPROPERTY(BlueprintReadWrite)
	bool bIsExist;
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
	int32 DataFragmentNum = 0;
	// 숨는 오브젝트 상태 (오브젝트 ID → 존재여부)
	UPROPERTY()
	TMap<FGuid, bool> HideObjectStates;
	// 문 상태 (문 ID → 잠김여부, 존재여부)
	UPROPERTY()
	TMap<FGuid, FDoorState> DoorStates;
	// 아이템 박스 존재 상태 (아이템박스 ID → 열림여부)
	UPROPERTY()
	TMap<FGuid, bool> ItemBoxStates;
	// 맵 내 아이템 존재여부 (아이템 ID → 존재여부)
	UPROPERTY()
	TMap<FGuid, bool> ItemStates;
	// AI 스폰 상태 (AI ID → 존재여부)
	UPROPERTY()
	TMap<FGuid, bool> AIStates;
	// 스캐너 상태(스캐너 ID → 활성화여부)
	UPROPERTY()
	bool ScannerStates = false;
};
