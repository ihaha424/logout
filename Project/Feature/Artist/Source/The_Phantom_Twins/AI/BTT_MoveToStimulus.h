#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToStimulus.generated.h"

// 태스크 인스턴스별 메모리 구조체 선언
USTRUCT()
struct FBTMoveToStimulusMemory
{
	GENERATED_BODY()

	FVector TargetLocation = FVector::ZeroVector;
	bool bPathValid = false;
};

UCLASS()
class THE_PHANTOM_TWINS_API UBTT_MoveToStimulus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_MoveToStimulus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTMoveToStimulusMemory); }

	UPROPERTY(EditAnywhere, Category = "AI")
	float MoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius = 5.f; // 도착 판정 거리
};
