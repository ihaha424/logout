// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "The_Phantom_Twins/Player/CharacterType.h"
#include "TPTSaveGameData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()

public:
    // 플레이어를 고유하게 식별할 ID (네트워크 ID 또는 계정 이름 등)
    UPROPERTY()
    FString PlayerID;

    // 플레이어가 선택한 캐릭터 정보
    UPROPERTY()
    ECharacterType SelectedCharacter;

    // 플레이어의 마지막 체크포인트 위치 및 회전값
    UPROPERTY()
    FVector PlayerLocation;

    UPROPERTY()
    FRotator PlayerRotation;

    // 기본값으로 초기화하는 생성자
    FPlayerSaveData()
    {
        PlayerID = TEXT("");
        SelectedCharacter = ECharacterType::None;
        PlayerLocation = FVector::ZeroVector;
        PlayerRotation = FRotator::ZeroRotator;
    }
};