// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BT_TPT_CompositeNode.h"
#include "BTC_RandomSelector.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTC_RandomSelector : public UBT_TPT_CompositeNode
{
	GENERATED_BODY()

public:
	UBTC_RandomSelector();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Random")
	bool bUseWeights = false;

	UPROPERTY(EditAnywhere, Category = "Random", meta = (EditCondition = "bUseWeights"))
	TArray<int32> Weights;

	mutable TMap<uint32, TArray<uint8>> InstanceShuffles;

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
	virtual void NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const override;
	virtual void NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
};
