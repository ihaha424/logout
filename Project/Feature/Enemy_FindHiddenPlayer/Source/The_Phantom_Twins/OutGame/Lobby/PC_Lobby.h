// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../Chapter/IdentifyChracterData.h"
#include "PC_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API APC_Lobby : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Lobby")
	void C2S_SelectChractor(const ECharacterType& type, const FName& DataName);
	void C2S_SelectChractor_Implementation(const ECharacterType& type, const FName& DataName);

	
};
