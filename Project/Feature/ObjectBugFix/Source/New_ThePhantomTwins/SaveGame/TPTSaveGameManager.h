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

    void ReInitialize();
    virtual void Deinitialize() override;

    UFUNCTION()
    void SaveUpdate();

    UFUNCTION()
    void InitializeSaveTargets();

    UFUNCTION(BlueprintCallable)
	void TempSaveByID(const FName ObjectID, const bool bIsExist);

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
    UWorld* CurrentWorld = nullptr;

    UPROPERTY()
    TArray<UTPTLocalPlayerSaveGame*> PlayerSaveGames;
    UPROPERTY()
    UTPTSaveGame* GameSaveGame;
    UPROPERTY()
    TMap<FName, AActor*> DoorActorsMap;
    UPROPERTY()
    TMap<FName, AActor*> ItemActorsMap;
    UPROPERTY()
    TMap<FName, AActor*> HideObjectActorsMap;
    UPROPERTY()
    TMap<FName, AActor*> ItemBoxActorsMap;
    UPROPERTY()
    TMap<FName, AActor*> AIActorsMap;
};
