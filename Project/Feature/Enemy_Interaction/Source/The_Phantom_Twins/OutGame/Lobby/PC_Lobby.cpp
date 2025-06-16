// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Lobby.h"
#include "GS_Lobby.h"
#include "../../StoryFlow/StoryFlowManager.h"


void APC_Lobby::C2S_SelectChractor_Implementation(const ECharacterType& type, const FName& DataName)
{
    bool bIsHost;

	UStoryFlowManager* Manager = GetGameInstance()->GetSubsystem<UStoryFlowManager>();

    UIdentifyChracterData* ChractorData = Manager->GetDataAs<UIdentifyChracterData>(DataName);
    if (!ChractorData)
    {
        UE_LOG(LogStoryFlow, Error, TEXT("GetDataAs<UIdentifyChracterData> Faild. Data name is %s"), *DataName.ToString());
        return;
    }

    if (IsLocalController())
    {
        if (ChractorData->Client == type)
            return;
        bIsHost = true;
    }
    else
    {
        if (ChractorData->Host == type)
            return;
        bIsHost = false;
    }

    AGameStateBase* GS = GetWorld()->GetGameState();
    AGS_Lobby* LobbyGS = Cast<AGS_Lobby>(GS);
    if (LobbyGS)
    {
        LobbyGS->S2A_SelectChractorEffect(type, bIsHost, DataName);
    }
}

