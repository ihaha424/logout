// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GS_Lobby.h"
#include "PS_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APS_Lobby : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
