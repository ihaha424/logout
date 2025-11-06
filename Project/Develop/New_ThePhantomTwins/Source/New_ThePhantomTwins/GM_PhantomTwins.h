// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserCountGameMode.h"
#include "GM_PhantomTwins.generated.h"

class ABossSpawner;

UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_PhantomTwins : public AUserCountGameMode
{
	GENERATED_BODY()
	
public:
	AGM_PhantomTwins();

    //~ Begin AGameModeBase
    virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AGameModeBase
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    UFUNCTION(BlueprintCallable)
    void ReInitializeGameSave();

	// ~ Begin PlayerDied
	void NotifyPlayerDied(bool isDead);
    // ~ End PlayerDied

    //~ Begin LevelTravel
    UFUNCTION()
	void DelayedInitializeSaveTargets();

    UFUNCTION(BlueprintCallable)
	void SeverToLevel(const FName LevelName, bool bAbsolute, bool bIsListen = true);

    UFUNCTION(BlueprintCallable)
    void ShowLoadingScene();
	void RestartThisLevel();

    UFUNCTION(BlueprintCallable)
	void ResumePlay();
	//~ End LevelTravel

    //~ Begin BossSpawn
    UPROPERTY(EditAnywhere, Category = "BossSpawn", meta = (ClampMin = "0.0"))
    float TimeLimitSeconds = 900.f;

    UPROPERTY(EditAnywhere, Category = "BossSpawn", meta = (ClampMin = "0"))
    int32 RequiredItemCount = 3;

    UPROPERTY(EditAnywhere, Category = "BossSpawn")
    TSoftObjectPtr<ABossSpawner> PreferredSpawnActor;
    //~ End BossSpawn

    // ~ Begin ReStart
    UFUNCTION()
    void NotifyPlayerClickRestart(bool bIsHostClicked, bool bIsClientClicked);
    void ShowGameOverUI();

	int32 TotalPlayerCount = 0;
    int32 DeadPlayerCount = 0;
    int32 HostClick = 0;
    int32 ClientClick = 0;
    // ~ End ReStart

	// ~ Begin Stop game
    UFUNCTION()
    void NotifyPlayerClickedGameStop(FName LevelName);
    void ShowGameStopUI();

    UFUNCTION()
    void NotifyPlayerAgreeWithGameStop(int32 HostSelect, int32 ClientSelect);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void PlayerTravel(FName DestinationLevel);
    void PlayerTravel_Implementation(FName DestinationLevel);
    void ShowResumeCountUI();

    FName DestinationLevelName;
    // ~ End  Stop game

protected:
    //~ Begin BossSpawn
    FTimerHandle TimerHandle_SpawnByTime;
    FDelegateHandle ItemChangedHandle;
    void OnItemCountChanged(int32 NewCount);
    void OnTimeLimitReached();
    void RequestBossSpawn();
    //~ End BossSpawn

    void SetAllPlayerUIMode(bool bIsUIMode);
};
