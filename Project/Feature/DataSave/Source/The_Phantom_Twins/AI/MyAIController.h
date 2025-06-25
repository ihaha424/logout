// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MyAIController.generated.h"

class AMyAICharacter;
class ASplinePathActor;
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
	virtual void Tick(float DeltaTime) override;
	void ResetStimulus();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// 감지 콜백 함수
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void PlayerPerception(AActor* Actor, FAIStimulus Stimulus);
	void ObjectPerception(AActor* Actor, FAIStimulus Stimulus);
	void AllyPerception(AActor* Actor, FAIStimulus Stimulus);
	ASplinePathActor* FindNearestSplinePath(const FVector& StimulusLocation);
	void CalculateSoundStimulus(FName Tag);

private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerception;
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
	UPROPERTY(EditAnywhere, Category = "AI")
	float HearingMaxPoint = 100;
	UPROPERTY(EditAnywhere, Category = "AI")
	float SightMaxTime = 1.1f;

	// 비헤이비어 트리와 블랙보드
	UPROPERTY()
	TObjectPtr<UBehaviorTree> BTAI;
	UPROPERTY()
	TObjectPtr<UBlackboardData> BBAI;

	UPROPERTY()
	bool bSeeingPlayer = false;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SeenDuration;
	UPROPERTY()
	TArray<FAuditoryStimulus> HearingStimulus;
	UPROPERTY(EditAnywhere, Category = "AI")
	float ExpireTime = 10.f;

	float SightStartTime = -1.0f;
	float LastHeardTime = -1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SightForgetTime = 3.0f; // 감지 해제 후 몇 초 뒤에 잊을지 설정
	float CurrentTime = 0.f;

	FVector PrevLocation;
	FVector CurrLocation;
	const float StimulusUpdateDistance = FMath::Square(100.0f);
};
