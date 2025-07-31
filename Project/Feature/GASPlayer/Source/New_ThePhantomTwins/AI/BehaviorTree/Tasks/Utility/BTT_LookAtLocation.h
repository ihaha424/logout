// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_LookAtLocation.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Move | LookAtLocation"))
class NEW_THEPHANTOMTWINS_API UBTT_LookAtLocation : public UBTT_GASBaseTask
{
	GENERATED_BODY()
	
public:
	UBTT_LookAtLocation();
	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};

