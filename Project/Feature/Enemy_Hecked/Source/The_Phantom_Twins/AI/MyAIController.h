// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyAIController.generated.h"

struct FAuditoryStimulus;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;
class UBlackboardData;
/**
 * 
 */
UCLASS()

class THE_PHANTOM_TWINS_API AMyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AMyAIController();

	void RunAI();
	void StopAI();
	virtual void Tick(float DeltaTime);
	//TODO:: private 으로 변경학...
	float LastSightStartTime = -1.0f;
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// 감지 콜백 함수
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// 비헤이비어 트리와 블랙보드
	UPROPERTY()
	TObjectPtr<UBehaviorTree> BTAI;

	UPROPERTY()
	TObjectPtr<UBlackboardData> BBAI;

	UPROPERTY()


	bool bSeeingPlayer = false;

	UPROPERTY()
	TArray<FAuditoryStimulus> HearingStimulus;
	UPROPERTY(EditAnywhere)
	float ExpireTime = 10.f;

	float AccumulatedHearingStrength = 0.f;
};
