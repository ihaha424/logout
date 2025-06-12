// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/CCTVManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UCCTVManager::UCCTVManager()
{
	PrimaryComponentTick.bCanEverTick = false;

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

	HackedIDSet.Add(CCTVId);
}

void UCCTVManager::RemoveHackedCCTV(int32 CCTVId)
{
	if (!HackedIDSet.Contains(CCTVId)) return;

	HackedIDSet.Remove(CCTVId);
}

bool UCCTVManager::HasHackedCCTV() const
{
	return HackedIDSet.Num() > 0;
}

ACCTV* UCCTVManager::GetPrevHackedCCTV(int32 CurrentCCTVId) const
{
	if (HackedIDSet.Num() == 0) return nullptr;

	TArray<int32> SortedIDs = HackedIDSet.Array();
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
	if (HackedIDSet.Num() == 0) return nullptr;

	TArray<int32> SortedIDs = HackedIDSet.Array();
	SortedIDs.Sort();

	int32 Index = SortedIDs.IndexOfByKey(CurrentCCTVId);
	if (Index == INDEX_NONE) return nullptr;

	int32 NextIndex = (Index == SortedIDs.Num() - 1) ? 0 : Index + 1;
	int32 NextID = SortedIDs[NextIndex];

	const TObjectPtr<ACCTV>* CCTVPtr = CCTVMap.Find(NextID);
	return CCTVPtr ? CCTVPtr->Get() : nullptr;
}
