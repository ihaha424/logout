// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_PseudoRespawn.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Ability | PseudoRespawn"))
class NEW_THEPHANTOMTWINS_API UBTT_PseudoRespawn : public UBTT_GASBaseTask
{
	GENERATED_BODY()
	
public:
	UBTT_PseudoRespawn();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RespawnLocationKey;

	UPROPERTY(EditAnywhere, Category = "Respawn")
	float RespawnDelay = 5.f;

private:
	void CompleteRespawn(ACharacter* Character, UBehaviorTreeComponent* OwnerComp, FVector Location);
};
