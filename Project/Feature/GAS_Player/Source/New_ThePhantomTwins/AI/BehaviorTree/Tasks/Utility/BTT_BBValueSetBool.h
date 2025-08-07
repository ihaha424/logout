// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_BBValueSetBool.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Utility | BBValueSetBool"))
class NEW_THEPHANTOMTWINS_API UBTT_BBValueSetBool : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_BBValueSetBool();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BBKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool Value;
};
