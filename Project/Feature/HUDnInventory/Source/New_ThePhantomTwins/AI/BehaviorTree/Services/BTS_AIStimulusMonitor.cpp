// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_AIStimulusMonitor.h"
#include "BehaviorTree/BlackboardComponent.h"

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

inline void UBTS_AIStimulusMonitor::AccumulationAttenuationOfSensory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds, UBlackboardComponent* BB)
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
        SightDuration = FMath::Max(0.f, SightDuration - SightDecayRate * DeltaSeconds);
    }
    BB->SetValueAsFloat(SightDurationKey.SelectedKeyName, SightDuration);

    // HearingSum 감쇠
    float HearingSum = BB->GetValueAsFloat(HearingSumKey.SelectedKeyName);
    float LastHearingTime = BB->GetValueAsFloat(LastHearingTimeKey.SelectedKeyName);
    float TimeSinceHearing = CurrentTime - LastHearingTime;

    if (TimeSinceHearing > 0.2f)
    {
        HearingSum = FMath::Max(0.f, HearingSum - HearingDecayPerSecond * DeltaSeconds);
        BB->SetValueAsFloat(HearingSumKey.SelectedKeyName, HearingSum);
    }

    DetermineStateTransition(BB, CurrentState, SightDuration, HearingSum, TimeSinceSight, TimeSinceHearing);
}

inline void UBTS_AIStimulusMonitor::DetermineStateTransition(UBlackboardComponent* BB
    , EAIBaseState CurrentState
    , float SightDuration
    , float HearingSum
    , float TimeSinceSight
    , float TimeSinceHearing)
{
    // 상태 전이 판단
    if (CurrentState == EAIBaseState::Default)
    {
        if (SightDuration >= SightCombatThreshold)
        {
            BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIBaseState::Combat));
        }
        else if ((SightDuration >= SightSuspicionThreshold && SightDuration < SightCombatThreshold) || HearingSum >= HearingSuspicionThreshold)
        {
            BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIBaseState::Suspicion));
        }
    }
    else if (CurrentState == EAIBaseState::Suspicion)
    {
        if (SightDuration >= SightCombatThreshold)
        {
            BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIBaseState::Combat));
        }
    }
    else if (CurrentState == EAIBaseState::Combat)
    {
        if (SightDuration <= 0.f && HearingSum <= 0.f && TimeSinceSight >= CombatTimeout && TimeSinceHearing >= CombatTimeout)
        {
            BB->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIBaseState::Default));
        }
    }
}
