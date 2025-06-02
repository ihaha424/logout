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

	if (!bSeeingPlayer || !Blackboard) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float SeenDuration = CurrentTime - LastSightStartTime;

	// 상태 전이 체크
	if (SeenDuration >= 1.1f)
	{
		uint8 CurrentState = Blackboard->GetValueAsEnum("AIState");
		if (CurrentState != static_cast<uint8>(EMyAIState::Combat))
		{
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
		}
	}
}

void AMyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 플레이어 감지가 아니면 무시
	APlayerBase* Target = Cast<APlayerBase>(Actor);
	if (!Target)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			bSeeingPlayer = true;

			Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
			Blackboard->SetValueAsObject(TEXT("TargetPlayer"), Target); // 이걸 Combat 진입에 사용

			if (LastSightStartTime < 0.0f)
			{
				LastSightStartTime = GetWorld()->GetTimeSeconds();
			}
		}
		else
		{
			bSeeingPlayer = false;
			LastSightStartTime = -1.0f;

			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));
		}
	}
	// --- 소리 감지 처리 ---
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AccumulatedHearingStrength += Stimulus.Strength;

			if (AccumulatedHearingStrength >= 100.f)
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				Blackboard->SetValueAsObject("TargetPlayer", Target);
			}
		}
	}
	// 감지 해제 시 초기화
	TArray<AActor*> PerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	if (!PerceivedActors.Contains(Actor))
	{
		bSeeingPlayer = false;
		LastSightStartTime = -1.0f;
		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		Blackboard->ClearValue(TEXT("TargetPlayer"));
	}
}
