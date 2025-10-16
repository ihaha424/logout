// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TPTSaveGameManager.generated.h"

struct FDoorState;
class UTPTLocalPlayerSaveGame;
class UTPTSaveGame;

UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UFUNCTION()
    void SaveUpdate();
    // 게임 시작 시 모든 저장 대상 액터 정보 초기화하는 함수
    UFUNCTION(BlueprintCallable)
    void InitializeSaveTargets();
    // 상태 변화 발생 시 Guid로 임시 저장함수 호출
    UFUNCTION(BlueprintCallable)
	void TempSaveByID(const FGuid& ObjectID, const bool bIsExist);
    UFUNCTION(BlueprintCallable)
    void TempSavePlayer(const APlayerController* PC, const bool bIsHost);
	UFUNCTION(BlueprintCallable)
    void SaveRestartPoint(const FVector& PlayerLocation, const FRotator& PlayerRotation);
	void LoadSaveGame();
	void ApplySaveData();

private:
    UPROPERTY()
    TArray<UTPTLocalPlayerSaveGame*> PlayerSaveGames;
    UPROPERTY()
    UTPTSaveGame* GameSaveGame;
    UPROPERTY()
    TMap<FGuid, AActor*> DoorActors;
    UPROPERTY()
    TMap<FGuid, AActor*> ItemActors;
    UPROPERTY()
    TMap<FGuid, AActor*> HideObjectActors;
    UPROPERTY()
    TMap<FGuid, AActor*> ItemBoxActors;
    UPROPERTY()
    TMap<FGuid, AActor*> AIActors;
};
