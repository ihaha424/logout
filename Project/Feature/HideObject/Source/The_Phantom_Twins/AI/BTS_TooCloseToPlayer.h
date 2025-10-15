// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_TooCloseToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTS_TooCloseToPlayer : public UBTService
{
	GENERATED_BODY()

public:
    UBTS_TooCloseToPlayer();
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MaxDistance = 200;
};
