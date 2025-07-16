// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AISenseRestoration.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Stun | SenseRestoration"))
class THE_PHANTOM_TWINS_API UBTT_AISenseRestoration : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_AISenseRestoration();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
