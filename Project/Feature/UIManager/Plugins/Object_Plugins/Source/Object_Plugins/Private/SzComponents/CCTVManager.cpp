// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/CCTVManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogCameraManger);

// Sets default values for this component's properties
UCCTVManager::UCCTVManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	CCTVList.SetNum(InitializeCameraMaxSize);
}


void UCCTVManager::BeginPlay()
{
	Super::BeginPlay();
}

void UCCTVManager::AddCCTV(int32 CCTVId, ACCTV* CCTV)
{
	if (!CCTV) return;
	
	if (CCTVId < 0)
	{
		UE_LOG(LogCameraManger, Error, TEXT("AddCCTV: CCTVId is InVaild."));
	}
	else if (!CCTVList.IsValidIndex(CCTVId))
	{
		CCTVList.SetNum(CCTVId);
	}

	CCTVList[CCTVId].CCTV = CCTV;
	CCTVList[CCTVId].bIsHacking = false;
	CCTVList[CCTVId].bIsUsed = false;
}

ACCTV* UCCTVManager::GetCCTV(int32 CCTVId) const
{
	if (!CCTVList.IsValidIndex(CCTVId))
	{
		UE_LOG(LogCameraManger, Error, TEXT("GetCCTV: CCTVId is InVaild."));
		return nullptr;
	}

	return CCTVList[CCTVId].CCTV;
}

void UCCTVManager::SetHackedCCTV(int32 CCTVId, bool bIsHacking)
{
	if (!CCTVList.IsValidIndex(CCTVId))
	{
		UE_LOG(LogCameraManger, Error, TEXT("SetHackedCCTV: CCTVId is InVaild."));
		return;
	}

	CCTVList[CCTVId].bIsHacking = bIsHacking;
}

void UCCTVManager::SetUsedCCTV(int32 CCTVId, bool bIsUsed)
{
	if (!CCTVList.IsValidIndex(CCTVId))
	{
		UE_LOG(LogCameraManger, Error, TEXT("SetUsedCCTV: CCTVId is InVaild."));
		return;
	}

	CCTVList[CCTVId].bIsUsed = bIsUsed;
}

int32 UCCTVManager::HasHackedCCTV() const
{
	int32 Count = 0;
	for (const auto& CCTVInfo : CCTVList)
	{
		if (CCTVInfo.bIsHacking)
			Count++;
	}
	return Count;
}

ACCTV* UCCTVManager::GetFirstHackedCCTV() const
{
	for (const auto& CCTVInfo : CCTVList)
	{
		if (CCTVInfo.bIsHacking && !CCTVInfo.bIsUsed)
			return CCTVInfo.CCTV;
	}
	return nullptr;
}

ACCTV* UCCTVManager::GetPrevHackedCCTV(int32 CurrentCCTVId) const
{
	int CurIndex = CurrentCCTVId;
	for (int32 count = 0; count < CCTVList.Num(); count++)
	{
		CurIndex--;
		if (CurIndex < 0)
			CurIndex = CCTVList.Num() - 1;
		if (CCTVList[CurIndex].bIsHacking && !CCTVList[CurIndex].bIsUsed)
		{
			return CCTVList[CurIndex].CCTV;
		}
	}
	return nullptr;
}

ACCTV* UCCTVManager::GetNextHackedCCTV(int32 CurrentCCTVId) const
{
	int CurIndex = CurrentCCTVId;
	for (int32 count = 0; count < CCTVList.Num(); count++)
	{
		CurIndex++;
		if (CurIndex >= CCTVList.Num())
			CurIndex = (CurIndex) % CCTVList.Num();
		if (CCTVList[CurIndex].bIsHacking && !CCTVList[CurIndex].bIsUsed)
			return CCTVList[CurIndex].CCTV;
	}
	return nullptr;
}

ACCTV* UCCTVManager::GetHackedCCTV(int32 CCTVId) const
{
	if (!CCTVList.IsValidIndex(CCTVId))
	{
		UE_LOG(LogCameraManger, Error, TEXT("GetHackedCCTV: CCTVId is InVaild."));
		return nullptr;
	}
	return CCTVList[CCTVId].CCTV;
}

void UCCTVManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCCTVManager, CCTVList);
}
