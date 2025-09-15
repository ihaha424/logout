// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_PhantomTwins.generated.h"

class ABossSpawner;

UCLASS()
class NEW_THEPHANTOMTWINS_API AGM_PhantomTwins : public AGameModeBase
{
	GENERATED_BODY()
	
public:

    //~ Begin AGameModeBase
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AGameModeBase


    //~ Begin LevelTravel
	void SeverToLevel(const FName LevelName, bool bAbsolute);
    //~ End LevelTravel


    //~ Begin BossSpawn
    UPROPERTY(EditAnywhere, Category = "BossSpawn", meta = (ClampMin = "0.0"))
    float TimeLimitSeconds = 900.f;

    UPROPERTY(EditAnywhere, Category = "BossSpawn", meta = (ClampMin = "0"))
    int32 RequiredItemCount = 3;

    UPROPERTY(EditAnywhere, Category = "BossSpawn")
    TSoftObjectPtr<ABossSpawner> PreferredSpawnActor;
    //~ End BossSpawn

protected:
    //~ Begin BossSpawn
    FTimerHandle TimerHandle_SpawnByTime;
    FDelegateHandle ItemChangedHandle;
    void OnItemCountChanged(int32 NewCount);
    void OnTimeLimitReached();
    void RequestBossSpawn();
    //~ End BossSpawn


};
