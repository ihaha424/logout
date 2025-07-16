// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTAISense_Hearing.h"
#include "Perception/AIPerceptionSystem.h"

float UTPTAISense_Hearing::Update()
{
    AIPerception::FListenerMap& ListenersMap = *GetListeners();
    UAIPerceptionSystem* PerceptionSys = GetPerceptionSystem();
    const float SpeedOfSoundSqScalar = SpeedOfSoundSq > 0.f ? 1.f / SpeedOfSoundSq : 0.f;

    if (NoiseEvents.Num() == 0)
        return SuspendNextUpdate;

    // 1. NoiseEvents 중 우선순위 높은 자극 1개만 선택
    const FAINoiseEvent* BestEvent = nullptr;
    int32 MaxPriority = -1;

    for (const FAINoiseEvent& Event : NoiseEvents)
    {
        const int32 Priority = 1; // GetPriorityFromTag(Event.Tag);
        if (Priority > MaxPriority)
        {
            MaxPriority = Priority;
            BestEvent = &Event;
        }
    }

    // 2. BestEvent가 없다면 끝냄
    if (!BestEvent)
    {
        NoiseEvents.Reset();
        return SuspendNextUpdate;
    }

    // 3. 선택된 BestEvent 하나만 처리
    for (AIPerception::FListenerMap::TIterator ListenerIt(ListenersMap); ListenerIt; ++ListenerIt)
    {
        FPerceptionListener& Listener = ListenerIt->Value;

        // skip listeners not interested in this sense
        if (!Listener.HasSense(GetSenseID()))
            continue;

        const FDigestedHearingProperties& PropDigest = DigestedProperties[Listener.GetListenerID()];

        const float ClampedLoudness = FMath::Max(0.f, BestEvent->Loudness);
        const FVector::FReal DistToSoundSquared = FVector::DistSquared(BestEvent->NoiseLocation, Listener.CachedLocation);

        // Limit by loudness modified squared range (this is the old behavior)
        if (DistToSoundSquared > PropDigest.HearingRangeSq * FMath::Square(ClampedLoudness))
            continue;

        // Limit by max range
        if (BestEvent->MaxRange > 0.f && DistToSoundSquared > FMath::Square(BestEvent->MaxRange * ClampedLoudness))
            continue;

        if (!FAISenseAffiliationFilter::ShouldSenseTeam(Listener.TeamIdentifier, BestEvent->TeamIdentifier, PropDigest.AffiliationFlags))
            continue;

        // calculate delay and fake it with Age, Delay should be pretty small so a static_cast is safe enough here.
        const float Delay = FloatCastChecked<float>(
            FMath::Sqrt(DistToSoundSquared * SpeedOfSoundSqScalar),
            UE::LWC::DefaultFloatPrecision
        );
        // pass over to listener to process 
        PerceptionSys->RegisterDelayedStimulus(
            Listener.GetListenerID(),
            Delay,
            BestEvent->Instigator,
            FAIStimulus(*this, ClampedLoudness, BestEvent->NoiseLocation, Listener.CachedLocation, FAIStimulus::SensingSucceeded, BestEvent->Tag)
        );
    }

    NoiseEvents.Reset();
    return SuspendNextUpdate;
}
