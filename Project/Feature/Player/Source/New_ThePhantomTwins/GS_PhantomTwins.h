// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_PhantomTwins.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGS_PhantomTwins : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float	GameTime = 0.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	int		CoreCount = 0;
};
