// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PhantomTwinsInstance.generated.h"

struct FPlayerSaveData;
class UStoryFlowManager;

UCLASS()
class THE_PHANTOM_TWINS_API UPhantomTwinsInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Story")
	UStoryFlowManager* GetStoryFlowManager();

	UPROPERTY()
	TArray<FPlayerSaveData> RestartPlayerDatas;

	// 재시작 모드인지 나타내는 플래그
	bool bIsRestartingLevel = false;
};
