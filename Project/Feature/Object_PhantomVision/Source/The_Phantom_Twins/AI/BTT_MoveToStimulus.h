#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToStimulus.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API UBTT_MoveToStimulus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_MoveToStimulus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FVector); }

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 5.f; // 원하는 범위

private:
	FVector TargetLocation;
	bool bPathValid = false;
};
