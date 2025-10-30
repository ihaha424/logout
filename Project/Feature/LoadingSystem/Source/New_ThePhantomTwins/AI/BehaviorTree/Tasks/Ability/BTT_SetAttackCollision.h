// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_SetAttackCollision.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Ability | SetAttackCollision"))
class NEW_THEPHANTOMTWINS_API UBTT_SetAttackCollision : public UBTT_GASBaseTask
{
	GENERATED_BODY()
public:
	UBTT_SetAttackCollision();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool SetValue = false;
};
