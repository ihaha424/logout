// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Lobby.h"
#include "GM_Lobby.h"


void APC_Lobby::SelectChractor(const ECharacterType& type)
{
	if (HasAuthority())
	{
		if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
		{
			if (AGM_Lobby* GM_Lobby = Cast<AGM_Lobby>(GM))
			{
				GM_Lobby->SetIdentifyCharacterData(type, true);
			}
		}
	}
	else
	{
		C2S_SelectChractor(type, false);
	}
}


void APC_Lobby::C2S_SelectChractor_Implementation(const ECharacterType& type, bool bIsHost)
{
	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
		if (AGM_Lobby* GM_Lobby = Cast<AGM_Lobby>(GM))
		{
			GM_Lobby->SetIdentifyCharacterData(type, bIsHost);
		}
	}
}
