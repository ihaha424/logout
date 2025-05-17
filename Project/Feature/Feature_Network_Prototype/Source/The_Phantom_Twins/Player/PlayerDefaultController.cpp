// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDefaultController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Components/WidgetComponent.h"


APlayerDefaultController::APlayerDefaultController()
{
	// Create AIPerceptionComponent & Setting
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

    // 시야 설정 추가
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 1000.0f;
        SightConfig->LoseSightRadius = 1200.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.f);     
        SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;

        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;


        Perception->ConfigureSense(*SightConfig);
        Perception->SetDominantSense(SightConfig->GetSenseImplementation());
    }

	// 청각 설정 추가
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    if (HearingConfig)
	{
		HearingConfig->HearingRange = 5000.f;
		HearingConfig->SetMaxAge(3.f);

		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

		Perception->ConfigureSense(*HearingConfig);
    }

    Perception->OnTargetPerceptionUpdated.AddDynamic(this, &APlayerDefaultController::OnTargetPerceptionUpdated);
}

void APlayerDefaultController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (HasAuthority()) 
    {
        S2C_UpdatePerceivedActor(Actor, Stimulus.WasSuccessfullySensed());
    }
}

void APlayerDefaultController::S2C_UpdatePerceivedActor_Implementation(AActor* Actor, bool bVisible)
{
    if (nullptr == Actor)
        return;

    // Add/Delete Object Array
    if (bVisible)
    {
        PerceptionActors.AddUnique(Actor);
    }
    else
    {
        PerceptionActors.Remove(Actor);
    }
    // Set Object UI
    if (UWidgetComponent* Widget = Actor->FindComponentByClass<UWidgetComponent>())
    {
        Widget->SetVisibility(bVisible);
    }
}
