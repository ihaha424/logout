// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Lobby.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API UW_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();


	UFUNCTION(BlueprintCallable)
	void ExitGame(const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void TravelToLevel(const FName LevelName);

	UFUNCTION(Server, Reliable)
	void Server_TravelToLevel(const FName& LevelName);
	void Server_TravelToLevel_Implementation(const FName& LevelName);

	void OpenAndTravelToLevel(const FName LevelName);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Flow Data")
	FName IdentifyChractor;
};
