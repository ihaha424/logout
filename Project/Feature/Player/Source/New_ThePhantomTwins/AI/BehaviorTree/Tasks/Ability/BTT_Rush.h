// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Tasks/BTT_GASBaseTask.h"
#include "BTT_Rush.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Ability | Rush"))
class NEW_THEPHANTOMTWINS_API UBTT_Rush : public UBTT_GASBaseTask
{
	GENERATED_BODY()
public:
	UBTT_Rush();

	virtual EBTNodeResult::Type Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float InitialSpeed = 500.f;
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float Acceleration = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float MaxSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StunKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HitDetectedKey;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	float* CurrentSpeedPtr = nullptr;
	FVector	RushDirection;
};
