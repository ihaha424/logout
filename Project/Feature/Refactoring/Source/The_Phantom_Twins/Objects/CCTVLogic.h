// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCTVLogic.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API ACCTVLogic : public APawn
{
	GENERATED_BODY()
	
public:	
	ACCTVLogic();

protected:
	virtual void BeginPlay() override;

public:	
	bool EnterFirstHackedCCTV(APawn* Interactor);
	void SetWidget(APlayerController* PC);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> NoHackedCCTVWidget;
	
	TObjectPtr<class UUserWidget> CCTVWidgetInstance = nullptr;


private:
	TObjectPtr<class APhantomTwinsGameState> CurrentGameState;
	TObjectPtr<class UCCTVManager> CurrentCCTVManager;


};
