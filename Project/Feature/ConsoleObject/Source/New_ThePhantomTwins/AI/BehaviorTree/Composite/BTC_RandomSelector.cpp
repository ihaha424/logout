// Fill out your copyright notice in the Description page of Project Settings.


#include "BTC_RandomSelector.h"

/**
 * @brief 
		:Things that I didn't think needed to be made into ".h" because they are only used in this space.
 */

int32 SelectRandomIndexByWeight(const TArray<int32>& Weights);

UBTC_RandomSelector::UBTC_RandomSelector()
{
	NodeName = TEXT("Random Selector");
	bUseNodeActivationNotify = true;
	bUseNodeDeactivationNotify = true;
}

int32 UBTC_RandomSelector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	int32 InstanceIdx = SearchData.OwnerComp.GetActiveInstanceIdx();
	int32& CurrentIndex = *(int32*)GetCompositeNodeMemory(SearchData, this, InstanceIdx);
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized || LastResult == EBTNodeResult::Failed)
	{
		TArray<uint8>& ShuffledIndices = InstanceShuffles[SearchData.OwnerComp.GetActiveInstanceIdx()];
		if (CurrentIndex < ShuffledIndices.Num())
		{
			int32 NextChildIndex = ShuffledIndices[CurrentIndex++];
			return NextChildIndex;
		}
	}

	return NextChildIdx;
}

void UBTC_RandomSelector::NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const
{
	int32 InstanceIdx = SearchData.OwnerComp.GetActiveInstanceIdx();
	int32& CurrentIndex = *(int32*)GetCompositeNodeMemory(SearchData, this, InstanceIdx);
	const int32 NumChildren = GetChildrenNum();

	TArray<uint8>& ShuffledIndices = InstanceShuffles.FindOrAdd(SearchData.OwnerComp.GetActiveInstanceIdx());
	ShuffledIndices.Empty();
	CurrentIndex = 0;

	TArray<int32> WorkingWeights = Weights;
	if (!bUseWeights || WorkingWeights.Num() != NumChildren)
	{
		WorkingWeights.Init(1, NumChildren);
	}

	TSet<int32> SelectedIndices;

	while (SelectedIndices.Num() < NumChildren)
	{
		int32 Next = SelectRandomIndexByWeight(WorkingWeights);
		if (!SelectedIndices.Contains(Next))
		{
			ShuffledIndices.Add(Next);
			SelectedIndices.Add(Next);
		}
	}
}

void UBTC_RandomSelector::NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const
{
	InstanceShuffles.Remove(SearchData.OwnerComp.GetActiveInstanceIdx());
}

uint16 UBTC_RandomSelector::GetInstanceMemorySize() const
{
	return GetMyInstanceMemorySize(sizeof(int32));
}

FString UBTC_RandomSelector::GetStaticDescription() const
{
	return TEXT("Randomly selects one child at a time until one succeeds or all fail.");
}

int32 SelectRandomIndexByWeight(const TArray<int32>& Weights)
{
	const int32 TotalWeight = Weights.Num() > 0 ? Algo::Accumulate(Weights, 0) : 0;

	if (TotalWeight <= 0)
		return FMath::RandRange(0, Weights.Num() - 1);

	int32 RandValue = FMath::RandRange(1, TotalWeight);
	int32 Accum = 0;

	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		Accum += Weights[i];
		if (RandValue <= Accum)
			return i;
	}

	return Weights.Num() - 1;
}