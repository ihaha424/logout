// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayAttackMontage.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTT_PlayAttackMontage : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_PlayAttackMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
