// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsGameState.h"
#include "Net/UnrealNetwork.h"

APhantomTwinsGameState::APhantomTwinsGameState()
{

}

void APhantomTwinsGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CCTVManager = NewObject<UCCTVManager>(this, UCCTVManager::StaticClass());
	CCTVManager->RegisterComponent();
}

void APhantomTwinsGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APhantomTwinsGameState, CCTVManager);
}
