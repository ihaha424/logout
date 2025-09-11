// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGame/HubMap/GM_HubMap.h"
#include "Kismet/GameplayStatics.h"


void AGM_HubMap::ExitGame(const FName LevelName)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC->HasAuthority())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* OtherPC = It->Get())
			{
				OtherPC->ClientTravel(LevelName.ToString(), TRAVEL_Absolute);
			}
		}
		UGameplayStatics::OpenLevel(GetWorld(), LevelName, true);
	}
	else
	{
		PC->ClientTravel(LevelName.ToString(), TRAVEL_Absolute);
	}
}

void AGM_HubMap::TravelToLevel(const FName LevelName, bool bAbsolute)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC->HasAuthority()) return;

	FString LevelPathWithListen = LevelName.ToString() + TEXT("?listen");

	UE_LOG(LogTemp, Log, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
	GetWorld()->ServerTravel(LevelPathWithListen, bAbsolute);
}
