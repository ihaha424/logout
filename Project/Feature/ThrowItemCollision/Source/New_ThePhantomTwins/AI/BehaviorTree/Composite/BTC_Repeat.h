// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BT_TPT_CompositeNode.h"
#include "BTC_Repeat.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UBTC_Repeat : public UBT_TPT_CompositeNode
{
	GENERATED_BODY()
public:
	UBTC_Repeat();

protected:
	UPROPERTY(EditAnywhere, Category = "Repeat")
	int32 MaxRepeats = 3;

	UPROPERTY(EditAnywhere, Category = "Repeat")
	bool bIsLoop = false;

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	virtual void NotifyNodeActivation(FBehaviorTreeSearchData& SearchData) const override;
};
