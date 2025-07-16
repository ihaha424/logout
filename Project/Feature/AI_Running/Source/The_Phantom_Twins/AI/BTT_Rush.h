// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Rush.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "TPTTask | Combat | Rush"))
class THE_PHANTOM_TWINS_API UBTT_Rush : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Rush();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	/**
	 * @brief	: Save the Speed(CurrentSpeed) for NodeMemory
	 * @return	: sizeof(float)
	 */
	virtual uint16 GetInstanceMemorySize() const override;


protected:
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float InitialSpeed = 500.f;
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float Acceleration = 200.f;
	UPROPERTY(EditAnywhere, Category = "AI | Status")
	float MaxSpeed = 1000.f;

private:
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	float*	CurrentSpeedPtr = nullptr;
	bool	bHitDetected = false;
	FVector	RushDirection;
};
