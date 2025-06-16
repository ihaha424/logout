// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "AIInterface.h"
#include "EngineUtils.h"
#include "MyAICharacter.h"
#include "SplinePathActor.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/PawnSensingComponent.h"
#include "SzObjects/HackableObject.h"
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
	SightConfig->PeripheralVisionAngleDegrees = 100.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1200.f;
	HearingConfig->SetMaxAge(10.f);
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
	// 플레이어가 움직임이 없어도 시야에 있는지 지속적인 검사를 위해서 쓴 코드.
	AActor* TargetActor = Cast< AActor>(Blackboard->GetValueAsObject(TEXT("TargetPlayer")));
	bSeeingPlayer = PerceptionComponent->HasActiveStimulus(*TargetActor, UAISense::GetSenseID<UAISense_Sight>());

	const float TickCurrentTime = GetWorld()->GetTimeSeconds();
	if (bSeeingPlayer && SightStartTime > 0.f)
	{
		SeenDuration = TickCurrentTime - SightStartTime;
		if (SeenDuration >= 1.1f)
		{
			uint8 CurrentState = Blackboard->GetValueAsEnum("AIState");
			if (CurrentState != static_cast<uint8>(EMyAIState::Combat))
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				Blackboard->SetValueAsObject(TEXT("ChasingPlayer"), TargetActor);
			}
		}
	}
	// 오래된 자극 제거
	for (int32 i = HearingStimulus.Num() - 1; i >= 0; --i)
	{
		if (TickCurrentTime - HearingStimulus[i].Timestamp > ExpireTime)
		{
			AccumulatedHearingStrength -= HearingStimulus[i].Strength;
			HearingStimulus.RemoveAt(i);
		}
	}
	// 새로운 추가 자극으로 부터 forgettime이 지나지 않았다면 여전히 봤다고 판단하도록 하기.
	const bool bStillSeeing = (SightStartTime > 0.f) && (TickCurrentTime - SightStartTime <= SightForgetTime) && bSeeingPlayer;
	uint8 CurrentState = Blackboard->GetValueAsEnum("AIState");

	/*지금 문제가 시야자극에서 갱신이 안되기 떄문에..*/


	// 전투상태 중이었는데 보이는것도 들리는것도 없으면 해제 하기.
	if(CurrentState == static_cast<uint8>(EMyAIState::Combat)&& !bStillSeeing && HearingStimulus.IsEmpty())
	{
		// 감지 시간이 모두 만료되었으면 상태 초기화
		SightStartTime = -1.0f;
		LastHeardTime = -1.0f;
		HearingStimulus.Empty();
		AccumulatedHearingStrength = 0.f;

		Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), FVector::ZeroVector);
		Blackboard->SetValueAsVector(TEXT("UsingStimulusLocation"), FVector::ZeroVector);
		Blackboard->SetValueAsEnum("AIStimulus", static_cast<uint8>(EMyAIStimulus::None));
		Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		AICharacter->S2A_UpdateAIStateWidget(EAIStateWidget::NoneMark);
		Blackboard->ClearValue("ChasingPlayer");
		Blackboard->ClearValue("TargetPlayer");
	}
}

void AMyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	AICharacter = Cast<AMyAICharacter>(GetPawn());
	if (!AICharacter || !AICharacter->AIStateWidget)
	{
		return;
	}
	CurrentTime = GetWorld()->GetTimeSeconds();

	if (Cast<APlayerBase>(Actor))
		PlayerPerception(Actor,Stimulus);

	else if (Cast<AMyAICharacter>(Actor))
		AllyPerception(Actor, Stimulus);

	else
		ObjectPerception(Actor, Stimulus);
}

void AMyAIController::PlayerPerception(AActor* Actor, FAIStimulus Stimulus)
{
	APlayerBase* Target = Cast<APlayerBase>(Actor);
	// 플레이어에 대한 시각 자극 처리.
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// AI의 위젯UI를 모두가 볼수있도록 하는 부분.
			AICharacter->S2A_UpdateAIStateWidget(EAIStateWidget::QuestionMark);
			// 시야 감지가 시작된적이 없다면 시간체크 시작.
			if (SightStartTime < 0.f)
				SightStartTime = CurrentTime;
			// 시야 감지에 따른 블랙보드 키 값 변경.
			Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
			Blackboard->SetValueAsVector(TEXT("UsingStimulusLocation"), Stimulus.StimulusLocation);

			Blackboard->SetValueAsEnum("AIStimulus", static_cast<uint8>(EMyAIStimulus::Sight));
			Blackboard->SetValueAsObject(TEXT("TargetPlayer"), Target);
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));
		}
	}
	
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// 발자국 마다 소리자극의 위치가 많이 갱신되는 것을 방지하기 위해서 만든 변수들.
			PrevLocation = Blackboard->GetValueAsVector(TEXT("UsingStimulusLocation"));
			CurrLocation = Blackboard->GetValueAsVector(TEXT("LastStimulusLocation"));

			AICharacter->S2A_UpdateAIStateWidget(EAIStateWidget::ExclamationMark);
			LastHeardTime = CurrentTime;

			HearingStimulus.Add(FAuditoryStimulus(CurrentTime, Stimulus.Strength));
			AccumulatedHearingStrength += Stimulus.Strength;

			// 들어온 자극의 순위를 확인하는 코드. TODO:: 플레이어가 뛰는 것과 걷는것을 어떻게 구분?
			////일단 이거 나중에 처리 ㅎ가ㅣ///if (Blackboard->GetValueAsEnum("AIStimulus") >= static_cast<uint8>(EMyAIStimulus::PlayerRun))
			
				// 일단 들어온 자극을 집어넣는다.
				Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), Stimulus.StimulusLocation);
				// 패트롤해야하는 자극 위치와 들어온 자극의 거리가 차이나면 패트롤위치를 갱신한다.
				if (FVector::DistSquared(PrevLocation, CurrLocation) > StimulusUpdateDistance)
				{
					Blackboard->SetValueAsVector(TEXT("UsingStimulusLocation"), Stimulus.StimulusLocation);
				}
			

			Blackboard->SetValueAsObject("TargetPlayer", Target);
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));

			if (AccumulatedHearingStrength >= 100.f)
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				Blackboard->SetValueAsObject("ChasingPlayer", Target);
			}
		}
	}
	// 자극의 근원지에서 가장 가까운 스플라인 경로를 찾는다.
	AICharacter->StimulusSplinePath = FindNearestSplinePath(Blackboard->GetValueAsVector(TEXT("UsingStimulusLocation")));
}

void AMyAIController::ObjectPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->GetClass()->ImplementsInterface(UHacking::StaticClass()))
	{
		AHackableObject* Object = Cast<AHackableObject>(Actor);
		if (Object->IHacking::CanBeHacked_Implementation())
		{
			Object->ClearHacking_Implementation();
		}
		
	}
}

void AMyAIController::AllyPerception(AActor* Actor, FAIStimulus Stimulus)
{
}

void AMyAIController::ResetStimulus()
{
	SightStartTime = -1.0f;
	LastHeardTime = -1.0f;
	HearingStimulus.Empty();
	AccumulatedHearingStrength = 0.f;
	Blackboard->SetValueAsVector(TEXT("LastStimulusLocation"), FVector::ZeroVector);
	Blackboard->SetValueAsVector(TEXT("UsingStimulusLocation"), FVector::ZeroVector);
	Blackboard->SetValueAsEnum("AIStimulus", static_cast<uint8>(EMyAIStimulus::None));
	Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));

	Blackboard->ClearValue("TargetPlayer");
	Blackboard->ClearValue("ChasingPlayer");
}

ASplinePathActor* AMyAIController::FindNearestSplinePath(const FVector& StimulusLocation)
{
	ASplinePathActor* ClosestSpline = nullptr;
	float ClosestDistance = FLT_MAX;
	for (TActorIterator<ASplinePathActor> It(GetWorld()); It; ++It)
	{
		ASplinePathActor* SplineActor = *It;
		if (SplineActor && SplineActor->SplineComponent)
		{
			FVector SplineStartLocation = SplineActor->SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
			float Dist = FVector::Dist(StimulusLocation, SplineStartLocation);
			if (Dist < ClosestDistance)
			{
				ClosestDistance = Dist;
				ClosestSpline = SplineActor;
			}
		}
	}
	return ClosestSpline;
}
