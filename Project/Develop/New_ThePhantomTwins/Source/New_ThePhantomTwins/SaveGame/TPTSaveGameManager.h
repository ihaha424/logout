// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TPTSaveGameManager.generated.h"

class ADataFragment;
class UGameplayEffect;
struct FDoorState;
class UTPTLocalPlayerSaveGame;
class UTPTSaveGame;

UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    UTPTSaveGameManager();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION()
    void SaveUpdate();
    // 게임 시작 시 모든 저장 대상 액터 정보 초기화하는 함수
    UFUNCTION()
    void InitializeSaveTargets();
    // 상태 변화 발생 시 Guid로 임시 저장함수 호출
    UFUNCTION(BlueprintCallable)
	void TempSaveByID(const FGuid& ObjectID, const bool bIsExist);

    UFUNCTION()
    void TempSavePlayer(const APlayerController* PC);

	UFUNCTION()
    void SetRestartPoint(const ADataFragment* DataFragment);
	UFUNCTION()
    FTransform GetRestartPoint(const bool bIsHost);

    UFUNCTION()
	void ApplyActorSaveGame();
	void InitializeSavePlayer();

	UFUNCTION()
	void ApplyAuthorityPlayerSaveGame(APlayerController* PC);
	void ApplyPlayerEffect(UAbilitySystemComponent* ASC, int32 Data);

	bool bActorsInitialized = false;
	bool bPlayerInitialized = false;

    TSubclassOf<UGameplayEffect> CoreEnergyEffect;
private:
    UPROPERTY()
    TArray<UTPTLocalPlayerSaveGame*> PlayerSaveGames;
    UPROPERTY()
    UTPTSaveGame* GameSaveGame;
    UPROPERTY()
    TMap<FGuid, AActor*> DoorActorsMap;
    UPROPERTY()
    TMap<FGuid, AActor*> ItemActorsMap;
    UPROPERTY()
    TMap<FGuid, AActor*> HideObjectActorsMap;
    UPROPERTY()
    TMap<FGuid, AActor*> ItemBoxActorsMap;
    UPROPERTY()
    TMap<FGuid, AActor*> AIActorsMap;
};
