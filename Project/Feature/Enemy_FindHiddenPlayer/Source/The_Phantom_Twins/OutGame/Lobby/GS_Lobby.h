// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../../Chapter/IdentifyChracterData.h"
#include "GS_Lobby.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API AGS_Lobby : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Lobby")
	void S2A_SelectChractorEffect(ECharacterType ChractorType, bool bIsHost, const FName& DataName);
	void S2A_SelectChractorEffect_Implementation(ECharacterType ChractorType, bool bIsHost, const FName& DataName);
};
