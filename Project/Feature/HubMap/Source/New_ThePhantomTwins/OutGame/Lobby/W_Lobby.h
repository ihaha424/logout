// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GS_Lobby.h"
#include "W_Lobby.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UW_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();


	UFUNCTION(BlueprintCallable)
	void ExitGame(const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void TravelToLevel(const FName LevelName);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleSetIdentifyChracter(FIdentifyChracterData IdentifyChracterData);

};
