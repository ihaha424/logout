// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Lobby.h"
#include "Net/UnrealNetwork.h"

void AGS_Lobby::SetIdentifyChracterData(ECharacterType ChractorType, bool bIsHost)
{
    if (bIsHost)
    {
        if(ChractorType == ECharacterType::None || IdentifyChracterData.Client != ChractorType)
            IdentifyChracterData.Host = ChractorType;
    }
    else
    {
        if (ChractorType == ECharacterType::None || IdentifyChracterData.Host != ChractorType)
            IdentifyChracterData.Client = ChractorType;
    }
    OnRep_IdentifyChracterData();
}

void AGS_Lobby::OnRep_IdentifyChracterData()
{
    OnSetIdentifyChracterData.Broadcast(IdentifyChracterData);
}

void AGS_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_Lobby, IdentifyChracterData);
}
