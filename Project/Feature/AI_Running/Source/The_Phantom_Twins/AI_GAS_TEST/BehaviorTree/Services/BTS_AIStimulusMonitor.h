// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "../../Utility/AIBaseState.h"
#include "BTS_AIStimulusMonitor.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UBTS_AIStimulusMonitor : public UBTService
{
	GENERATED_BODY()

public:
    UBTS_AIStimulusMonitor();
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual FString GetStaticDescription() const override;

    //~ Begin Blackboard Key
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AIStateKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector SightDurationKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastSightTimeKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector HearingSumKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastHearingTimeKey;
    //~ End Blackboard Key

    //~ Begin Options related to sensory retention
    // 시야 감지 임계값
    UPROPERTY(EditAnywhere, Category = "Stimulus")
    float SightDetectionThreshold   = 0.2f;

    // 시야 누적 감쇠율
    UPROPERTY(EditAnywhere, Category = "Stimulus")  
    float SightDecayRate            = 1.0f;

    // 청각 누적 감쇠율
    UPROPERTY(EditAnywhere, Category = "Stimulus")  
    float HearingDecayPerSecond     = 50.f;
    
    // 전투 상태 시야 임계값
    UPROPERTY(EditAnywhere, Category = "Stimulus")  
    float SightCombatThreshold      = 1.1f;
    
    // 의심 상태 시야 임계값
    UPROPERTY(EditAnywhere, Category = "Stimulus")  
    float SightSuspicionThreshold   = 0.1f;

    // 의심 상태 청각 임계값
    UPROPERTY(EditAnywhere, Category = "Stimulus")
    float HearingSuspicionThreshold = 100.f;

    // 전투 지속 시간
    UPROPERTY(EditAnywhere, Category = "Stimulus")
    float CombatTimeout             = 5.0f;
    //~ End Options related to sensory retention

private:
    /**
    * @brief : 감각의 누적 & 감쇠
    */
    inline void AccumulationAttenuationOfSensory(UBehaviorTreeComponent& OwnerComp
                                        , uint8* NodeMemory
                                        , float DeltaSeconds
                                        , UBlackboardComponent* BB);
    /**
     * @brief : 상태 전이 판단
     */
    inline void DetermineStateTransition(UBlackboardComponent* BB
        , EAIBaseState CurrentState
        , float SightDuration
        , float HearingSum
        , float TimeSinceSight
        , float TimeSinceHearing);
};
