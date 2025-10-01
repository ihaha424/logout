// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGame/HubMap/GS_HubMap.h"
#include "Net/UnrealNetwork.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"
#include "Log/TPTLog.h"

void AGS_HubMap::SetCurState(EHubMapState State)
{
    PrevState = CurState;
    CurState = State;

    OnChangeHubMapState.Broadcast(PrevState, CurState);
}

void AGS_HubMap::SetIdentifyCharacterTypeData(ECharacterType CharacterType, bool bIsHost)
{
    if (bIsHost)
    {
        if (IdentifyCharacterData.Host == CharacterType)
            IdentifyCharacterData.Host = ECharacterType::None;
        else if (CharacterType == ECharacterType::None || IdentifyCharacterData.Client != CharacterType)
            IdentifyCharacterData.Host = CharacterType;
    }
    else
    {
        if (IdentifyCharacterData.Client == CharacterType)
            IdentifyCharacterData.Client = ECharacterType::None;
        else if (CharacterType == ECharacterType::None || IdentifyCharacterData.Host != CharacterType)
            IdentifyCharacterData.Client = CharacterType;
    }
    OnRep_IdentifyCharacterData();
}

void AGS_HubMap::SetIdentifyCharacterSkillData(ESkillType CharacterSkill, bool bIsHost)
{
    if (bIsHost)
    {
        if (IdentifyCharacterData.HostSkill == CharacterSkill)
            IdentifyCharacterData.HostSkill = ESkillType::NoneSkill;
        else
			IdentifyCharacterData.HostSkill = CharacterSkill;
    }
    else
    {
        if (IdentifyCharacterData.ClientSkill == CharacterSkill)
            IdentifyCharacterData.ClientSkill = ESkillType::NoneSkill;
        else
			IdentifyCharacterData.ClientSkill = CharacterSkill;
    }
    OnRep_IdentifyCharacterData();
}

void AGS_HubMap::SetCharacterReady(bool bIsReady, bool bIsHost)
{
    if (bIsHost)
    {
        bIsServerReady = bIsReady;
    }
    else
    {
        bIsClientReady = bIsReady;
    }

    OnRep_ReadyCharacterData();
}

void AGS_HubMap::OnRep_HubMapState()
{
    OnChangeHubMapState.Broadcast(PrevState, CurState);
    PrevState = CurState;
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

void AGS_HubMap::OnRep_ReadyCharacterData()
{
    OnSetReadyData.Broadcast(bIsServerReady, bIsClientReady);
}

void AGS_HubMap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_HubMap, IdentifyCharacterData);
    DOREPLIFETIME(AGS_HubMap, NextLevel);
    DOREPLIFETIME(AGS_HubMap, MapData);
    DOREPLIFETIME(AGS_HubMap, bIsServerReady);
    DOREPLIFETIME(AGS_HubMap, bIsClientReady);
    DOREPLIFETIME(AGS_HubMap, CurState);
}
