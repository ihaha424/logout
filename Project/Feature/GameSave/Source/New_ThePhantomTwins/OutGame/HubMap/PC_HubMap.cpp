// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGame/HubMap/PC_HubMap.h"
#include "OutGame/HubMap/GM_HubMap.h"
#include "Log/TPTLog.h"

void APC_HubMap::C2S_SelectCharacter_Implementation(const ECharacterType type)
{
	if (!HasAuthority())
		return;

	const bool bIsHost = IsLocalController();

	AGS_HubMap* GS = GetWorld()->GetGameState<AGS_HubMap>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_HubMap' Fail"));
		return;
	}

	GS->SetIdentifyCharacterTypeData(type, bIsHost);
}

void APC_HubMap::C2S_SelectSkill_Implementation(const ESkillType type)
{
	if (!HasAuthority())
		return;

	const bool bIsHost = IsLocalController();

	AGS_HubMap* GS = GetWorld()->GetGameState<AGS_HubMap>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_HubMap' Fail"));
		return;
	}

	GS->SetIdentifyCharacterSkillData(type, bIsHost);
}

void APC_HubMap::C2S_SelectState_Implementation(const EHubMapState type)
{
	if (!HasAuthority())
		return;

	AGS_HubMap* GS = GetWorld()->GetGameState<AGS_HubMap>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_HubMap' Fail"));
		return;
	}
	GS->SetCurState(type);
}

void APC_HubMap::C2S_SelectMap_Implementation(const FName& type, EMapType State)
{
	if (!HasAuthority())
		return;

	AGS_HubMap* GS = GetWorld()->GetGameState<AGS_HubMap>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_HubMap' Fail"));
		return;
	}
	GS->SetNextLevel(type);
	GS->SetMapType(State);
}

void APC_HubMap::C2S_LevelTravel_Implementation(const FName& type, bool bAbsolute)
{
	if (!HasAuthority())
		return;

	AGM_HubMap* GM = GetWorld()->GetAuthGameMode<AGM_HubMap>();
	if (!GM)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGM_HubMap' Fail"));
		return;
	}
	GM->TravelToLevel(type, bAbsolute);
}
