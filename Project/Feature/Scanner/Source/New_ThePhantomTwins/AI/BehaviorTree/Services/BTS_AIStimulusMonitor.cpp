// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_AIStimulusMonitor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Tags/TPTGameplayTags.h"
#include "AI/Utility/AIHelperLibrary.h"
#include "Log/TPTLog.h"

UBTS_AIStimulusMonitor::UBTS_AIStimulusMonitor()
{
	NodeName = TEXT("Monitor Stimulus & Handle State");

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTS_AIStimulusMonitor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    AccumulationAttenuationOfSensory(OwnerComp, NodeMemory, DeltaSeconds, BB);
}

FString UBTS_AIStimulusMonitor::GetStaticDescription() const
{
	return FString::Printf(TEXT("Determine AI state transitions and manage sensory levels."));
}

inline void UBTS_AIStimulusMonitor::AccumulationAttenuationOfSensory(
    UBehaviorTreeComponent& OwnerComp
    , uint8* NodeMemory
    , float DeltaSeconds
    , UBlackboardComponent* BB)
{
    const float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();
    EAIBaseState CurrentState = static_cast<EAIBaseState>(BB->GetValueAsEnum(AIStateKey.SelectedKeyName));

    // SightDuration 누적 or 감쇠
    float SightDuration = BB->GetValueAsFloat(SightDurationKey.SelectedKeyName);
    float LastSightTime = BB->GetValueAsFloat(LastSightTimeKey.SelectedKeyName);
    float TimeSinceSight = CurrentTime - LastSightTime;
    
    if (TimeSinceSight <= SightDetectionThreshold)
    {
        SightDuration += DeltaSeconds;
    }
    else
    {
        SightDuration -= SightDecayRate * DeltaSeconds;
    }
    SightDuration = FMath::Clamp(SightDuration, 0.f, MaxSightDuration);
    BB->SetValueAsFloat(SightDurationKey.SelectedKeyName, SightDuration);

    // HearingSum 감쇠
    float HearingSum = BB->GetValueAsFloat(HearingSumKey.SelectedKeyName);
    float LastHearingTime = BB->GetValueAsFloat(LastHearingTimeKey.SelectedKeyName);
    float TimeSinceHearing = CurrentTime - LastHearingTime;

    if (TimeSinceHearing > 0.2f)
    {
        HearingSum = FMath::Clamp(HearingSum - HearingDecayPerSecond * DeltaSeconds, 0.f, MaxHearingSum);

        BB->SetValueAsFloat(HearingSumKey.SelectedKeyName, HearingSum);
    }

    DetermineStateTransition(OwnerComp, BB, CurrentState, SightDuration, HearingSum, TimeSinceSight, TimeSinceHearing);
}

inline void UBTS_AIStimulusMonitor::DetermineStateTransition(
    UBehaviorTreeComponent& OwnerComp
    , UBlackboardComponent* BB
    , EAIBaseState CurrentState
    , float SightDuration
    , float HearingSum
    , float TimeSinceSight
    , float TimeSinceHearing)
{
    EAIBaseState NextState = CurrentState;

    // 상태 전이 판단
    if (CurrentState == EAIBaseState::Default)
    {
        /*
        // 소음 아이템과 플레이어 행동 소음에 할당된 포인트를 누적하여 의심상태로 변동
        // 시야 범위 내의 플레이어/적 액터를 인지한 시간이 1.0초 이하일 경우 의심 상태로 변경
        // 의심 상태로 변동시, 가장 마지막에 인지한 자극의 위치로 이동
        */
        if (BB->GetValueAsBool(StunKey.SelectedKeyName))
        {
            NextState = EAIBaseState::Stun;
        }
        else if (BB->GetValueAsBool(InCombatRangeKey.SelectedKeyName))
        {
            BB->SetValueAsBool(InCombatRangeKey.SelectedKeyName, false);
            NextState = EAIBaseState::Combat;
        }
        else if (SightDuration >= SightCombatThreshold)
        {
            NextState = EAIBaseState::Combat;
        }
        else if ((SightDuration >= SightSuspicionThreshold && SightDuration < SightCombatThreshold) 
            || HearingSum >= HearingSuspicionThreshold)
        {
            NextState = EAIBaseState::Suspicion;
        }
    }
    else if (CurrentState == EAIBaseState::Suspicion)
    {
        /*
        // 의심 상태로 변동 시, 가장 마지막에 인지한 자극의위치로 이동
        // 우선 순위가 높은 자극이 발생하면 새로운 자극의 위치로 이동
        // 완료 조건:
                Move to Stimulus Location 행동을 완료 할 떄까지 추가 소음 자극이 없으면 가장 가까운 Spluine Path 탐색을 시작(시야 자극이 발생하면 시야 자극 추적)
                Spline Path 탐색을 시작하면 추가 소음 자극을 업데이트하지 않음 (시야 자극이 발생하면 시야 자극 추적)
                Spline Path 탐색이 완료되면 저장한 자극 정보를 초기화하고 기본 상태로 변동
        */
        if (BB->GetValueAsBool(StunKey.SelectedKeyName))
        {
            NextState = EAIBaseState::Stun;
        }
        else if (BB->GetValueAsBool(InCombatRangeKey.SelectedKeyName))
        {
            BB->SetValueAsBool(InCombatRangeKey.SelectedKeyName, false);
            NextState = EAIBaseState::Combat;
        }
        else if (SightDuration >= SightCombatThreshold)
        {
            NextState = EAIBaseState::Combat;
        }
    }
    else if (CurrentState == EAIBaseState::Combat)
    {
        /*
        // 전투 상태면 소음 자극 무시
        // 추적 중에 자극이 사야 범위를 벗어날 경우 마지막으로 인식한 시야 자극의 위치로 이동
        // 완료 조건:
                시야 범위 내에 플레이어, 적이 위치 하지 않은 시간이 N초 이상할 때, 가장 가까운 Spline Path탐색
                Spline Path 탐색을 시작하면 추가 소음 자극을 업데이트 하지 않음(시야 자극이 발생하면 시야 자극 추적)
                Spline 탐색이 완료되면 저장한 자극 정보를 초기화 하고 기본 상태로 변동
        */
        if (BB->GetValueAsBool(StunKey.SelectedKeyName))
        {
            NextState = EAIBaseState::Stun;
        }
        else if (SightDuration <= 0.f && TimeSinceSight >= CombatTimeout)
        {
            NextState = EAIBaseState::Suspicion;
        }
        else
        {   // TargetActor가 없는 경우: 감각안의 플레이어가 죽어서 인지 하지 못하는 경우...
            UObject* Object = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
            if (nullptr == Object)
            {
                NextState = EAIBaseState::Suspicion;
            }
        }
    }
    else if (CurrentState == EAIBaseState::Stun)
    {
        // TPT_LOG(AILog, Log, TEXT("CurrentState == EAIBaseState::Stun: Nothing"));
    }
    else
    {
        TPT_LOG(AILog, Warning, TEXT("State: Failed."));
    }

    UAIHelperLibrary::SetAIStateAndTag(&OwnerComp, BB, CurrentState, NextState, AIStateKey.SelectedKeyName);
}
