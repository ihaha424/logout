// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTravelSystem/LevelTravelNetProxy.h"
#include "LevelTravelSystem/TravelManagerSubsystem.h"

// Sets default values
ALevelTravelNetProxy::ALevelTravelNetProxy()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ALevelTravelNetProxy::BeginPlay()
{
	Super::BeginPlay();


}

void ALevelTravelNetProxy::C2S_RequestTravelToLevel_Implementation(APlayerController* PC)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;

	UTravelManagerSubsystem* TravelManagerSubsystem = GameInstance->GetSubsystem<UTravelManagerSubsystem>();
	if (!TravelManagerSubsystem)
		return;
	TravelManagerSubsystem->CheckAllPlayerReady(PC);
}

void ALevelTravelNetProxy::S2C_ResponTravelToLevel_Implementation(APlayerController* PC)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;

	UTravelManagerSubsystem* TravelManagerSubsystem = GameInstance->GetSubsystem<UTravelManagerSubsystem>();
	if (!TravelManagerSubsystem)
		return;
	TravelManagerSubsystem->FinishTravel();
}
