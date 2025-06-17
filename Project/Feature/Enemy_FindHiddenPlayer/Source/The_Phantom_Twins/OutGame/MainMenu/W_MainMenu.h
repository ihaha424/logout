// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_MainMenu.generated.h"


class UButton;
class UBorder;
class UEditableTextBox;

UCLASS()
class THE_PHANTOM_TWINS_API UW_MainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();

	UFUNCTION(BlueprintCallable)
	void StartLobby(const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void ConnectLobby(const FName LevelName);
};
