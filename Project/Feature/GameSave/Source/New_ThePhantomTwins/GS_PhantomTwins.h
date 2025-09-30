// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/MapType.h"
#include "GS_PhantomTwins.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnCollectedItemCountChanged, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossSpawnedDynamic, AActor*, BossActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollectedItem, AActor*, DataFragment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedGameStop, FName , LevelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedRestart, bool, bIsHostClicked, bool, bIsClientClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedAgreeWithGameStop, int32, HostSelect, int32, ClientSelect);

UCLASS()
class NEW_THEPHANTOMTWINS_API AGS_PhantomTwins : public AGameStateBase
{
	GENERATED_BODY()
public:
	//~ Begin BossSpawn
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "BossSpawn")
	float	GameTime = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_CollectedItemCount, VisibleAnywhere, BlueprintReadWrite, Category = "BossSpawn")
	int		CoreCount = 0;
	UPROPERTY(ReplicatedUsing = OnRep_BossSpawned, BlueprintReadOnly, Category = "BossSpawn")
	bool	bBossSpawned = false;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BossSpawn")
	TObjectPtr<AActor> BossActor;
	UPROPERTY(BlueprintAssignable, Category = "BossSpawn")
	FOnBossSpawnedDynamic OnBossSpawned;

	FOnCollectedItemCountChanged& OnCollectedItemCountChanged() { return CollectedItemCountChanged; }
	UFUNCTION(BlueprintCallable, Category = "BossSpawn")
	void AddCollectedItem(AActor* DataFragment, int32 Delta = 1);
	void MarkBossSpawned(AActor* InBoss);
	//~ End BossSpawn

	//~ Begin MapData
	EMapType GetMapData() const { return MapData; }
	void SetMapData(EMapType mapType) { MapData = mapType; }
	//~ End MapData

	//~ Begin Restart
	UPROPERTY(BlueprintAssignable)
	FOnClickedRestart OnClickedRestartChanged;

	void SetCharacterClickedRestart(bool bIsClicked, bool bIsHost);
	UFUNCTION()
	void OnRep_SetCharacterClickedRestart();

	UPROPERTY(ReplicatedUsing = OnRep_SetCharacterClickedRestart)
	bool bIsHostClickedRestart = false;
	UPROPERTY(ReplicatedUsing = OnRep_SetCharacterClickedRestart)
	bool bIsClientClickedRestart = false;
	//~ End Restart

	// ~ Begin Stop game
	UPROPERTY(BlueprintAssignable)
	FOnClickedGameStop OnClickedGameStopChanged;
	UPROPERTY(BlueprintAssignable)
	FOnClickedAgreeWithGameStop OnClickedAgreeWithGameStopChanged;

	void SetCharacterClickedGameStop(FName LevelName);
	void SetCharacterAgreeWithGameStop(int32 bIsClicked, bool bIsHost);
	UFUNCTION()
	void OnRep_SetCharacterClickedGameStop();
	UFUNCTION()
	void OnRep_SetCharacterAgreeWithGameStop();

	UPROPERTY(ReplicatedUsing = OnRep_SetCharacterClickedGameStop)
	FName DestinationLevelName;
	UPROPERTY(ReplicatedUsing = OnRep_SetCharacterAgreeWithGameStop)
	int32 HostSelect = 0;
	UPROPERTY(ReplicatedUsing = OnRep_SetCharacterAgreeWithGameStop)
	int32 ClientSelect = 0;

	//~ End Stop game
protected:
	//~ Begin DataFragment
	UPROPERTY(BlueprintAssignable, Category = "DataFragment")
	FOnCollectedItem DataFragmentChanged;
	//~ End DataFragment

	//~ Begin BossSpawn
	FOnCollectedItemCountChanged CollectedItemCountChanged;
	UFUNCTION()
	void OnRep_BossSpawned();
	UFUNCTION()
	void OnRep_CollectedItemCount();
	//~ End BossSpawn

	//~ Begin MapData
	EMapType MapData;
	//~ End MapData

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;
};
