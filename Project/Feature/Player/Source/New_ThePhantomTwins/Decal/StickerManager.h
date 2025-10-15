// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Decal/StickerActor.h"
#include "GameFramework/Info.h"
#include "StickerManager.generated.h"


class AStickerActor;

USTRUCT()
struct FStickerRecord 
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AStickerActor> Actor;
	UPROPERTY()
	int32 OwnerPlayerId = -1;
	UPROPERTY()
	double ExpireAt = 0.0;       // UGameplayStatics::GetTimeSeconds 기반
	UPROPERTY()
	FIntVector Cell;             // 영역 버킷 키
};

UCLASS()
class NEW_THEPHANTOMTWINS_API AStickerManager : public AInfo
{
	GENERATED_BODY()

public:
	AStickerManager() {};

	// == 정책 ==
	UPROPERTY(EditDefaultsOnly) 
	int32 MaxTotal = 300;
	UPROPERTY(EditDefaultsOnly) 
	int32 MaxPerPlayer = 30;
	UPROPERTY(EditDefaultsOnly) 
	int32 MaxPerCell = 50;
	UPROPERTY(EditDefaultsOnly) 
	float CellSize = 2000.f;
	UPROPERTY(EditDefaultsOnly) 
	float LifeTime = 600.f;

	//// == 통계(복제) ==
	//UPROPERTY(ReplicatedUsing = OnRep_Stats) 
	//int32 TotalCount = 0;
	/*UPROPERTY(ReplicatedUsing = OnRep_Stats) 
	TMap<int32, int32> PerPlayerCount;*/

	// == 서버 전용 상태 ==
	TArray<FStickerRecord> Active;                 // 전체
	TMap<FIntVector, TArray<int32>> CellToIdx;     // 버킷(인덱스 리스트)
	TMultiMap<double, int32> ExpireMinHeap;         // (ExpireAt -> ActiveIdx) 간단히 multimap로 구현

	// 관리
//	virtual void Tick(float DeltaSeconds) override;
//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//
//protected:
//	UFUNCTION() void OnRep_Stats();
//
//	bool CanPlace(int32 PlayerId, const FIntVector& CellKey) const;
//	void RegisterSticker(AStickerActor* NewActor, int32 PlayerId, double ExpireAt, const FIntVector& CellKey);
//	void RemoveStickerByIndex(int32 ActiveIdx);
//
//	FIntVector ToCellKey(const FVector& Loc) const;
};
