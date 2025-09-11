// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/MapType.h"
#include "GS_PhantomTwins.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnCollectedItemCountChanged, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossSpawnedDynamic, AActor*, BossActor);

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
	void AddCollectedItem(int32 Delta = 1);
	void MarkBossSpawned(AActor* InBoss);
	//~ End BossSpawn

	//~ Begin MapData
	EMapType GetMapData() const { return MapData; }
	void SetMapData(EMapType mapType) { MapData = mapType; }
	//~ End MapData

protected:
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
