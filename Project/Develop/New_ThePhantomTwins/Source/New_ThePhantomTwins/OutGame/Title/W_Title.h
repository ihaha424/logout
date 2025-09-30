// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Title.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UW_Title : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void StartLobby(const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void ConnectLobby(const FName LevelName);
};
