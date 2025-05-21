// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"

#include "AIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMyAIController::AMyAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAIRef(TEXT("/Script/AIModule.BlackboardData'/Game/ThirdPerson/AI/BB_AI.BB_AI'"));
	if (BBAIRef.Object != nullptr)
	{
		BBAI = BBAIRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAIRef(TEXT("/Script/AIModule.BehaviorTree'/Game/ThirdPerson/AI/BT_AI.BT_AI'"));
	if (BTAIRef.Object != nullptr)
	{
		BTAI = BTAIRef.Object;
	}

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->SetMaxAge(5.f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIController::OnTargetPerceptionUpdated);

}

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();
	UBlackboardComponent* BBComponent = Blackboard.Get();
	if (UseBlackboard(BBAI, BBComponent))
	{
		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to use blackboard"));
	}

}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}


void AMyAIController::RunAI()
{
	UBlackboardComponent* BBComponent = Blackboard.Get();
	if (UseBlackboard(BBAI,BBComponent))
	{
		Blackboard->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		bool RunResult = RunBehaviorTree(BTAI);
		ensure(RunResult);
	}
}

void AMyAIController::StopAI()
{

}

void AMyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
		Blackboard->SetValueAsObject(TEXT("Player"), Actor);
	}
	else
	{
		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		Blackboard->ClearValue(TEXT("Player"));
	}
}

