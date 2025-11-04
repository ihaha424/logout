// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadWatcher.generated.h"

/**
 * 
 */
UCLASS()
class ALoadWatcher : public AActor
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void StartPreloadOnce();
    UFUNCTION()
    void PollPreload();

    bool bPreloadStarted = false;
    bool bReadySent = false;
    FTimerHandle PollHandle;
};
