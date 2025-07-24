// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Lobby.h"
#include "Net/UnrealNetwork.h"

void AGS_Lobby::SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost)
{
    if (bIsHost)
    {
        if(ChractorType == ECharacterType::None || IdentifyCharacterData.Client != ChractorType)
            IdentifyCharacterData.Host = ChractorType;
    }
    else
    {
        if (ChractorType == ECharacterType::None || IdentifyCharacterData.Host != ChractorType)
            IdentifyCharacterData.Client = ChractorType;
    }
    OnRep_IdentifyCharacterData();
}

void AGS_Lobby::OnRep_IdentifyCharacterData()
{
    OnSetIdentifyCharacterData.Broadcast(IdentifyCharacterData);
}

void AGS_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_Lobby, IdentifyCharacterData);
}
