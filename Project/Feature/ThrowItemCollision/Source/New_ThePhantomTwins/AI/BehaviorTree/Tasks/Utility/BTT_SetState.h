// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BTT_GASBaseTask.h"
#include "../../../Utility/AIBaseState.h"
#include "BTT_SetState.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Utility | SetState"))
class NEW_THEPHANTOMTWINS_API UBTT_SetState : public UBTT_GASBaseTask
{
	GENERATED_BODY()

public:
	UBTT_SetState();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AIStateKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	EAIBaseState StateValue;
};
