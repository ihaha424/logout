// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TPTSaveGameManager.generated.h"

class UTPTLocalPlayerSaveGame;
class UTPTSaveGame;

UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    //// 플레이어별 세이브 게임 객체 배열 (2명 분)
    //UPROPERTY()
    //TArray<UTPTLocalPlayerSaveGame*> PlayerSaveGames;

    //// 전체 게임 세이브 게임 객체
    //UPROPERTY()
    //UTPTSaveGame* GameSaveGame;
    //virtual void Initialize(FSubsystemCollectionBase& Collection) override;


    //// 저장 함수 예시: 특정 시점에 호출
    //UFUNCTION()
    //void SaveAll();


    //// 불러오기 함수 예시
    //UFUNCTION()
    //void LoadAll();

};
