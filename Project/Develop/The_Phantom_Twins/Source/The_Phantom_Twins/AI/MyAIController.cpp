// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"

#include "AIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

AMyAIController::AMyAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAIRef(TEXT("/Script/AIModule.BlackboardData'/Game/Temp/BB_AI.BB_AI'"));
	if (BBAIRef.Object != nullptr)
	{
		BBAI = BBAIRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAIRef(TEXT("/Script/AIModule.BehaviorTree'/Game/Temp/BT_AI.BT_AI'"));
	if (BTAIRef.Object != nullptr)
	{
		BTAI = BTAIRef.Object;
	}

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;
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

void AMyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (bSeeingPlayer && LastSightStartTime > 0.f)
	{
		float SeenDuration = CurrentTime - LastSightStartTime;
		if (SeenDuration >= 1.1f)
		{
			uint8 CurrentState = Blackboard->GetValueAsEnum("AIState");
			if (CurrentState != static_cast<uint8>(EMyAIState::Combat))
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				//UE_LOG(LogTemp, Warning, TEXT("Player seen for %.2f seconds. Switching to Combat."), SeenDuration);
			}
		}
	}
	// 오래된 자극 제거
	for (int32 i = HearingStimulus.Num() - 1; i >= 0; --i)
	{
		if (CurrentTime - HearingStimulus[i].Timestamp > ExpireTime)
		{
			AccumulatedHearingStrength -= HearingStimulus[i].Strength;
			HearingStimulus.RemoveAt(i);
			UE_LOG(LogTemp, Error, TEXT("Delete Delete Delete Delete: %f"), AccumulatedHearingStrength);
		}
	}

	// 시야 기반 상태 전이 유지
	const bool bStillSeeing = (LastSeenTime > 0.f) && (CurrentTime - LastSeenTime <= ForgetTime);

	const bool bStillHearing = (LastHeardTime > 0.f) && (CurrentTime - LastHeardTime <= ForgetTime);


	uint8 CurrentState = Blackboard->GetValueAsEnum("AIState");
	if (bStillSeeing || bStillHearing)
	{
		if (CurrentState != static_cast<uint8>(EMyAIState::Combat) && CurrentState != static_cast<uint8>(EMyAIState::Hacked))
		{
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
		}
	}
	else if(CurrentState != static_cast<uint8>(EMyAIState::Hacked))
	{
		// 감지 시간이 모두 만료되었으면 상태 초기화
		LastSeenTime = -1.0f;
		LastHeardTime = -1.0f;
		HearingStimulus.Empty();
		AccumulatedHearingStrength = 0.f;

		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		Blackboard->ClearValue("TargetPlayer");
	}
}

void AMyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 플레이어 감지가 아니면 무시
	APlayerBase* Target = Cast<APlayerBase>(Actor);
	if (!Target) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			if (LastSightStartTime < 0.f)
				LastSeenTime = CurrentTime;

			Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
			Blackboard->SetValueAsObject(TEXT("TargetPlayer"), Target);
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));
		}
	}

	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			LastHeardTime = CurrentTime;

			HearingStimulus.Add(FAuditoryStimulus(CurrentTime, Stimulus.Strength));
			AccumulatedHearingStrength += Stimulus.Strength;

			if (AccumulatedHearingStrength >= 100.f)
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				Blackboard->SetValueAsObject("TargetPlayer", Target);
			}
		}
	}
}
