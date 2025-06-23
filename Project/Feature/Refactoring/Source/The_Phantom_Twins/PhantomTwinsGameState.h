// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Objects/CCTVManager.h"
#include "PhantomTwinsGameState.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API APhantomTwinsGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	APhantomTwinsGameState();

//protected:
//	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "CCTV")
	UCCTVManager* GetCCTVManager() const { return CCTVManager; }

protected:
	//UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<UCCTVManager> CCTVManager;
};
