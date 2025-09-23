// Fill out your copyright notice in the Description page of Project Settings.


#include "BTC_Repeat.h"

UBTC_Repeat::UBTC_Repeat()
{
	NodeName = TEXT("Repeat");
	bUseNodeActivationNotify = true;
}

int32 UBTC_Repeat::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	int32 InstanceIdx = SearchData.OwnerComp.GetActiveInstanceIdx();
	int32& RepeatPtr = *(int32*)GetCompositeNodeMemory(SearchData, this, InstanceIdx);
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		NextChildIdx = 0;
	}
	else if (LastResult == EBTNodeResult::Succeeded)
	{
		if ((PrevChild + 1) < GetChildrenNum())
		{
			NextChildIdx = PrevChild + 1;
		}
		else
		{
			++RepeatPtr;
			if (RepeatPtr < MaxRepeats || bIsLoop)
				NextChildIdx = 0;
			else
				NextChildIdx = BTSpecialChild::ReturnToParent;
		}
	}

	return NextChildIdx;
}

uint16 UBTC_Repeat::GetInstanceMemorySize() const
{
	return GetMyInstanceMemorySize(sizeof(int32));
}

FString UBTC_Repeat::GetStaticDescription() const
{
	return bIsLoop ? FString::Printf(TEXT("Repeat(Only First Node): Loop")) : FString::Printf(TEXT("Repeat(Only First Node): Until Fail or MaxCount: %d"), MaxRepeats);
}

void UBTC_Repeat::NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const
{
	int32 InstanceIdx = SearchData.OwnerComp.GetActiveInstanceIdx();
	int32& RepeatPtr = *(int32*)GetCompositeNodeMemory(SearchData, this, InstanceIdx);

	RepeatPtr = 0;
}
