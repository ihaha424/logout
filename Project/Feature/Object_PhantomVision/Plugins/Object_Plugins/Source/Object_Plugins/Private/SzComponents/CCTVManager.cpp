// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/CCTVManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCCTVManager::UCCTVManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UCCTVManager::BeginPlay()
{
	Super::BeginPlay();
}

void UCCTVManager::AddCCTV(int32 CCTVId, ACCTV* CCTV)
{
	if (!CCTV) return;
	CCTVMap.Add(CCTVId, CCTV);
}

void UCCTVManager::RemoveCCTV(int32 CCTVId)
{
	if (!CCTVMap.Contains(CCTVId)) return;
	CCTVMap.Remove(CCTVId);
	RemoveHackedCCTV(CCTVId);
}

void UCCTVManager::AddHackedCCTV(int32 CCTVId)
{
	if (!CCTVMap.Contains(CCTVId)) return;
	if (!HackedIDArray.Contains(CCTVId))
	{
		HackedIDArray.Add(CCTVId);
		// Replication: 서버에서만 직접 수정
	}
}

void UCCTVManager::RemoveHackedCCTV(int32 CCTVId)
{
	if (HackedIDArray.Contains(CCTVId))
	{
		HackedIDArray.Remove(CCTVId);
		// Replication: 서버에서만 직접 수정
	}
}

bool UCCTVManager::HasHackedCCTV() const
{
	return HackedIDArray.Num() > 0;
}

ACCTV* UCCTVManager::GetFirstHackedCCTV() const
{
	if (HackedIDArray.Num() == 0) return nullptr;

	TArray<int32> SortedIDs = HackedIDArray;
	SortedIDs.Sort();

	int32 cctvID = SortedIDs[0];
	const TObjectPtr<ACCTV>* firstHackedCCTV = CCTVMap.Find(cctvID);
	return firstHackedCCTV ? firstHackedCCTV->Get() : nullptr;
}

ACCTV* UCCTVManager::GetPrevHackedCCTV(int32 CurrentCCTVId) const
{
	if (HackedIDArray.Num() == 0) return nullptr;

	TArray<int32> SortedIDs = HackedIDArray;
	SortedIDs.Sort();

	int32 Index = SortedIDs.IndexOfByKey(CurrentCCTVId);
	if (Index == INDEX_NONE) return nullptr;

	int32 PrevIndex = (Index == 0) ? SortedIDs.Num() - 1 : Index - 1;
	int32 PrevID = SortedIDs[PrevIndex];

	const TObjectPtr<ACCTV>* CCTVPtr = CCTVMap.Find(PrevID);
	return CCTVPtr ? CCTVPtr->Get() : nullptr;
}

ACCTV* UCCTVManager::GetNextHackedCCTV(int32 CurrentCCTVId) const
{
	if (HackedIDArray.Num() == 0) return nullptr;

	TArray<int32> SortedIDs = HackedIDArray;
	SortedIDs.Sort();

	int32 Index = SortedIDs.IndexOfByKey(CurrentCCTVId);
	if (Index == INDEX_NONE) return nullptr;

	int32 NextIndex = (Index == SortedIDs.Num() - 1) ? 0 : Index + 1;
	int32 NextID = SortedIDs[NextIndex];

	const TObjectPtr<ACCTV>* CCTVPtr = CCTVMap.Find(NextID);
	return CCTVPtr ? CCTVPtr->Get() : nullptr;
}

void UCCTVManager::OnRep_HackedIDArray()
{
	// 클라이언트에서 HackedIDArray가 갱신될 때 필요한 동작이 있으면 여기에 구현
}

void UCCTVManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCCTVManager, HackedIDArray);
}