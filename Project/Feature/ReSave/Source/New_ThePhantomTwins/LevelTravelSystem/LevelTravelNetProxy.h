// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTravelNetProxy.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ALevelTravelNetProxy : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelTravelNetProxy();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void C2S_RequestTravelToLevel(APlayerController* PC);
	void C2S_RequestTravelToLevel_Implementation(APlayerController* PC);

	UFUNCTION(Client, Reliable)
	void S2C_ResponTravelToLevel(APlayerController* PC);
	void S2C_ResponTravelToLevel_Implementation(APlayerController* PC);
};
