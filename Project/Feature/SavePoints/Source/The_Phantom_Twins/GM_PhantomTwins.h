// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_PhantomTwins.generated.h"

struct FPlayerSaveData;
class UUserWidget;

UCLASS()
class THE_PHANTOM_TWINS_API AGM_PhantomTwins : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void NotifyPlayerDied(AController* DeadPlayer, bool isDead);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_ShowFadeUI();
	void S2A_ShowFadeUI_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeUI")
	TObjectPtr<UUserWidget> FadeUI;

protected:

	int32 DeadPlayerCount = 0;
	int32 TotalPlayerCount = 0;

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void RestartLevelWithDelay(float Delay);

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	// 게임 시작 시 GameInstance에서 데이터를 가져오기 위한 변수
	UPROPERTY()
	TArray<FPlayerSaveData> OverrideSpawnDatas;

	bool bShouldOverrideSpawns = false;
};
