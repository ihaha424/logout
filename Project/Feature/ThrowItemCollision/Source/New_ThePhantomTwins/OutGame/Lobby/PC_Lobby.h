// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/CharacterType.h"
#include "PC_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APC_Lobby : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SelectChractor(const ECharacterType& type);


protected:
	UFUNCTION(Server, Reliable)
	void C2S_SelectChractor(const ECharacterType& type, bool bIsHost);
	void C2S_SelectChractor_Implementation(const ECharacterType& type, bool bIsHost);
};
