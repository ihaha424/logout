// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AIIsHacked.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Stun | IsHacked"))
class THE_PHANTOM_TWINS_API UBTT_AIIsHacked : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_AIIsHacked();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
