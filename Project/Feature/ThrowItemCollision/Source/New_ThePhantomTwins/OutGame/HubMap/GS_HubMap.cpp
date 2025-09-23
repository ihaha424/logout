// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGame/HubMap/GS_HubMap.h"
#include "Net/UnrealNetwork.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"

void AGS_HubMap::SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost)
{
    if (bIsHost)
    {
        if (IdentifyCharacterData.Host == ChractorType)
            IdentifyCharacterData.Host = ECharacterType::None;
        else if (ChractorType == ECharacterType::None || IdentifyCharacterData.Client != ChractorType)
            IdentifyCharacterData.Host = ChractorType;
    }
    else
    {
        if (IdentifyCharacterData.Client == ChractorType)
            IdentifyCharacterData.Client = ECharacterType::None;
        else if (ChractorType == ECharacterType::None || IdentifyCharacterData.Host != ChractorType)
            IdentifyCharacterData.Client = ChractorType;
    }
    OnRep_IdentifyCharacterData();
}

void AGS_HubMap::OnRep_MapData()
{
    UTPTSaveGame* TPTLocalPlayerSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
    TPTLocalPlayerSaveGame->IdentifyMapData.MapType = MapData;
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTSaveGame>(TPTLocalPlayerSaveGame);
}

void AGS_HubMap::OnRep_IdentifyCharacterData()
{
    UTPTSaveGame* TPTLocalPlayerSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
    TPTLocalPlayerSaveGame->IdentifyCharacterData = IdentifyCharacterData;
    UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTSaveGame>(TPTLocalPlayerSaveGame);

    OnSetIdentifyCharacterData.Broadcast(IdentifyCharacterData);
}

void AGS_HubMap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_HubMap, IdentifyCharacterData);
    DOREPLIFETIME(AGS_HubMap, NextLevel);
    DOREPLIFETIME(AGS_HubMap, MapData);
}
