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

		Blackboard->SetValueAsVector(TEXT("PlayerStimulusLocation"), FVector::ZeroVector);
		Blackboard->SetValueAsVector(TEXT("UpdatedStimulusLocation"), FVector::ZeroVector);
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
	if (Target->IsGroggy()) return;

	if (Blackboard->GetValueAsVector(TEXT("UpdatedStimulusLocation")) == FVector::ZeroVector)
	{
		Blackboard->SetValueAsVector(TEXT("UpdatedStimulusLocation"), Stimulus.StimulusLocation);
	}
	Blackboard->SetValueAsVector(TEXT("PlayerStimulusLocation"), Stimulus.StimulusLocation);
	Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));
	// 플레이어에 대한 시각 자극 처리.
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("AI UAISense_Sight PlayerPerception : %s"), *Actor->GetName());
			// AI의 위젯UI를 모두가 볼수있도록 하는 부분.
			AICharacter->S2A_UpdateAIStateWidget(EAIStateWidget::QuestionMark);
			// 시야 감지가 시작된적이 없다면 시간체크 시작.
			if (SightStartTime < 0.f)
				SightStartTime = CurrentTime;
			// 시야 감지시 감각 업데이트 111111111111순위.
			Blackboard->SetValueAsEnum("AIStimulus", static_cast<uint8>(EMyAIStimulus::Sight));
			Blackboard->SetValueAsVector(TEXT("UpdatedStimulusLocation"), Stimulus.StimulusLocation);
			Blackboard->SetValueAsObject(TEXT("TargetPlayer"), Target);
		}
	}
	
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("AI UAISense_Hearing PlayerPerception : %s"), *Actor->GetName());
			// 발자국 마다 소리자극의 위치가 많이 갱신되는 것을 방지하기 위해서 만든 변수들.
			PrevLocation = Blackboard->GetValueAsVector(TEXT("UpdatedStimulusLocation"));
			CurrLocation = Blackboard->GetValueAsVector(TEXT("PlayerStimulusLocation"));

			AICharacter->S2A_UpdateAIStateWidget(EAIStateWidget::ExclamationMark);
			LastHeardTime = CurrentTime;

			HearingStimulus.Add(FAuditoryStimulus(CurrentTime, Stimulus.Strength));
			AccumulatedHearingStrength += Stimulus.Strength;

			// 들어온 자극의 순위를 확인하는 코드. TODO:: 플레이어가 뛰는 것과 걷는것을 어떻게 구분?
			////일단 이거 나중에 처리 ㅎ가ㅣ///if (Blackboard->GetValueAsEnum("AIStimulus") >= static_cast<uint8>(EMyAIStimulus::PlayerRun))
			
			// 일단 들어온 자극을 집어넣는다.
			Blackboard->SetValueAsVector(TEXT("PlayerStimulusLocation"), Stimulus.StimulusLocation);
			// 패트롤해야하는 자극 위치와 들어온 자극의 거리가 차이나면 패트롤위치를 갱신한다.
			if (FVector::DistSquared(PrevLocation, CurrLocation) > StimulusUpdateDistance)
			{
				UE_LOG(LogTemp, Warning, TEXT("UpdatedStimulusLocation is Updated by \"UAISense_Hearing\""));

				Blackboard->SetValueAsVector(TEXT("UpdatedStimulusLocation"), Stimulus.StimulusLocation);
			}

			Blackboard->SetValueAsObject("TargetPlayer", Target);

			if (AccumulatedHearingStrength >= 100.f)
			{
				Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
				Blackboard->SetValueAsObject("ChasingPlayer", Target);
			}
		}
	}
	// 자극의 근원지에서 가장 가까운 스플라인 경로를 찾는다.
	AICharacter->StimulusSplinePath = FindNearestSplinePath(Blackboard->GetValueAsVector(TEXT("UpdatedStimulusLocation")));
}

void AMyAIController::ObjectPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->GetClass()->ImplementsInterface(UHacking::StaticClass()))
	{
		AHackableObject* Object = Cast<AHackableObject>(Actor);
		// 오브제트가 이미 해킹이되어서 해킹이 불가한 상태라면 밑의 코드가 실행됨.
		if (!Object->IHacking::CanBeHacked_Implementation())
		{
			UE_LOG(LogTemp, Warning, TEXT("AI UAISense_Hearing ObjectPerception : %s"), *Actor->GetName());
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Suspicion));
			Blackboard->SetValueAsObject("TargetObject", Object);
		}
	}
}

void AMyAIController::AllyPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		AMyAICharacter* Ally = Cast<AMyAICharacter>(Actor);
		AMyAIController* AllyController = Cast<AMyAIController>(Ally->GetController());
		UBlackboardComponent* BlackboardComp = AllyController->GetBlackboardComponent();

		if (BlackboardComp->GetValueAsEnum("AIState") == static_cast<uint8>(EMyAIState::Combat))
		{
			UE_LOG(LogTemp, Warning, TEXT("AI UAISense_Sight AllyPerception : %s"), *Actor->GetName());
			Blackboard->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
		}
	}
}

void AMyAIController::ResetStimulus()
{
	SightStartTime = -1.0f;
	LastHeardTime = -1.0f;
	HearingStimulus.Empty();
	AccumulatedHearingStrength = 0.f;
	Blackboard->SetValueAsVector(TEXT("PlayerStimulusLocation"), FVector::ZeroVector);
	Blackboard->SetValueAsVector(TEXT("UpdatedStimulusLocation"), FVector::ZeroVector);
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
			// Spline 전체에서 StimulusLocation과 가장 가까운 Spline상의 점을 찾음
			USplineComponent* SplineComp = SplineActor->SplineComponent;
			float ClosestKey = SplineComp->FindInputKeyClosestToWorldLocation(StimulusLocation);
			FVector ClosestPointOnSpline = SplineComp->GetLocationAtSplineInputKey(ClosestKey, ESplineCoordinateSpace::World);

			float Dist = FVector::Dist(StimulusLocation, ClosestPointOnSpline);
			if (Dist < ClosestDistance)
			{
				ClosestDistance = Dist;
				ClosestSpline = SplineActor;
			}
		}
	}
	return ClosestSpline;
}
