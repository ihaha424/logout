// Fill out your copyright notice in the Description page of Project Settings.


#include "W_Lobby.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GS_Lobby.h"
#include "../../GM_PhantomTwins.h"

void UW_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	if (AGS_Lobby* GS = GetWorld()->GetGameState<AGS_Lobby>())
	{
		GS->OnSetIdentifyCharacterData.AddDynamic(this, &UW_Lobby::HandleSetIdentifyChracter);
	}
}

void UW_Lobby::ExitGame(const FName LevelName)
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

void UW_Lobby::TravelToLevel(const FName LevelName)
{
	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
		if (AGM_PhantomTwins* GM_TPT = Cast<AGM_PhantomTwins>(GM))
		{
			GM_TPT->SeverToLevel(LevelName, false);
		}
	}
}
