// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Decal/StickerActor.h"
#include "GameFramework/Info.h"
#include "StickerManager.generated.h"

USTRUCT()
struct FStickerRecord 
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AStickerActor> Actor;
	UPROPERTY()
	FStickerParams Params;
	UPROPERTY()
	int32 OwnerPlayerId = -1;
	UPROPERTY()
	double SpawnTime = 0.0;
	UPROPERTY()
	FRotator Rotation = FRotator();
	UPROPERTY()
	FVector Locattion = FVector();
};

/**
 * @brief 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AStickerManager : public AInfo
{
	GENERATED_BODY()

public:
	AStickerManager();

	UFUNCTION(BlueprintCallable)
	void RegisterSticker(int32 OwnerPlayerId, AStickerActor* NewActor);


protected:
	void EnforcePlayerLimit(int32 OwnerPlayerId);
	void AddSticker(int32 OwnerPlayerId, AStickerActor* NewActor);
	void RemoveOldestOf(int32 OwnerPlayerId);

protected:
	// == Policy ==
	UPROPERTY(EditDefaultsOnly)
	int32 MaxTotal = 300;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxPerPlayer = 5;
	UPROPERTY(EditDefaultsOnly)
	bool bBlock = false;

private:
	// == Data ==
	using PlayerIndex = int32;
	using StickerRecordArray = TArray<FStickerRecord>;
	TMap<PlayerIndex, StickerRecordArray> PerPlayerStickers;
};
