// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PhantomTwinsInstance.generated.h"

class UStoryFlowManager;

UCLASS()
class THE_PHANTOM_TWINS_API UPhantomTwinsInstance : public UGameInstance
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Story")
	UStoryFlowManager* GetStoryFlowManager();

public:
	UFUNCTION(BlueprintCallable)
	void LoadLevelWithLoadingScreen(const FString& LevelName);

private:
	void OnLevelLoaded();
};
