// Fill out your copyright notice in the Description page of Project Settings.


#include "W_Lobby.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PC_Lobby.h"

void UW_Lobby::NativeConstruct()
{
	Super::NativeConstruct();


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
		APC_Lobby* LobbyPC = Cast<APC_Lobby>(PC);
		if (LobbyPC)
		{
			LobbyPC->C2S_SelectChractor(ECharacterType::None, IdentifyChractor);
			
			LobbyPC->ClientTravel(LevelName.ToString(), TRAVEL_Absolute);
		}
	}
}

void UW_Lobby::TravelToLevel(const FName LevelName)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC)) return;

	if (PC->HasAuthority())
	{
		OpenAndTravelToLevel(LevelName);
	}
	else
	{
		// Features are provided, but calls are recommended only on hosts.
		Server_TravelToLevel(LevelName);
	}
}

void UW_Lobby::Server_TravelToLevel_Implementation(const FName& LevelName)
{
	OpenAndTravelToLevel(LevelName);
}

void UW_Lobby::OpenAndTravelToLevel(const FName LevelName)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC->HasAuthority()) return;

	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//{
	//	if (APlayerController* OtherPC = It->Get())
	//	{
	//		OtherPC->ClientTravel(LevelName.ToString(), TRAVEL_Absolute);
	//	}
	//}

	//UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, TEXT("listen"));

	FString LevelPathWithListen = LevelName.ToString() + TEXT("?listen");

	UE_LOG(LogTemp, Error, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
	GetWorld()->ServerTravel(LevelPathWithListen, true);
}
