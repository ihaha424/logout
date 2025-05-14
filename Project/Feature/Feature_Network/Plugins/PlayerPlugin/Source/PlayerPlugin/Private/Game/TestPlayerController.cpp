// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TestPlayerController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"


ATestPlayerController::ATestPlayerController()
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

    Perception->OnTargetPerceptionUpdated.AddDynamic(this, &ATestPlayerController::OnTargetPerceptionUpdated);
}

void ATestPlayerController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    UE_LOG(LogTemp, Warning, TEXT("Object"));

	if (UWidgetComponent* Widget = Actor->FindComponentByClass<UWidgetComponent>())
    {
        Widget->SetVisibility(Stimulus.WasSuccessfullySensed());
    }

    FString Temp = (("Perceived actor: %s"), *Actor->GetName());
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Temp); 
    UE_LOG(LogTemp, Warning, TEXT("gkatnghjcnf"));
}
