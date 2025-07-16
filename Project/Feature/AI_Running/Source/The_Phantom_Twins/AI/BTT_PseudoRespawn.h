// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PseudoRespawn.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | PseudoRespawn"))
class THE_PHANTOM_TWINS_API UBTT_PseudoRespawn : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTT_PseudoRespawn();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Respawn")
    FVector RespawnLocation;

    UPROPERTY(EditAnywhere, Category = "Respawn")
    float RespawnDelay;

private:
    void CompleteRespawn(ACharacter* Character, UBehaviorTreeComponent* OwnerComp);
};
