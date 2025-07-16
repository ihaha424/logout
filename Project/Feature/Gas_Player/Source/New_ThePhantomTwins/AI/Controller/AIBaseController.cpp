// AIBaseController.cpp

#include "AIBaseController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

#include "../Character/AIBaseCharacter.h"
#include "../Attributes/AIBaseAttributeSet.h"
#include "../Utility/SplineActor.h"

AAIBaseController::AAIBaseController()
{
    PrimaryActorTick.bCanEverTick = true;

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

    // Default field of view
    // Set in AIAttributeSet
    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 1800.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;

    HearingConfig->HearingRange = 1200.f;
    HearingConfig->SetMaxAge(3.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->ConfigureSense(*HearingConfig);
    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBaseController::OnTargetPerceptionUpdated);
}

void AAIBaseController::BeginPlay()
{
    Super::BeginPlay();
}

void AAIBaseController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
    bool bSeeingPlayer = PerceptionComponent->HasActiveStimulus(*TargetActor, UAISense::GetSenseID<UAISense_Sight>());
    if (bSeeingPlayer)
    {
        UBlackboardComponent* BB = GetBlackboardComponent();
        if (BB)
        {
            const float CurrentTime = GetWorld()->GetTimeSeconds();
            BB->SetValueAsFloat(TEXT("LastSightTime"), CurrentTime);
        }
    }
}

void AAIBaseController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (!BehaviorTree)
        return;
    RunBehaviorTree(BehaviorTree);
    UBlackboardData* BlackboardData = BehaviorTree->GetBlackboardAsset();
    if (!BlackboardData)
        return;
    UBlackboardComponent* BBComponent = Blackboard.Get();
    if (!BBComponent)
        return;
    if (!UseBlackboard(BlackboardData, BBComponent))
        return;
    AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(InPawn);
    if (!AIBaseCharacter)
        return;

    const FName DefaultSplineActor = TEXT("DefaultSplineActor");
    BBComponent->SetValueAsObject(DefaultSplineActor, AIBaseCharacter->GetBaseSplineActor());

    SetPerceptionByCharacterAttributeSet(InPawn);
}

void AAIBaseController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !GetBlackboardComponent()) return;

    UBlackboardComponent* BB = GetBlackboardComponent();
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            BB->SetValueAsObject(TEXT("TargetActor"), Actor);
            BB->SetValueAsVector(TEXT("StimulusLocation"), Stimulus.StimulusLocation);
            BB->SetValueAsFloat(TEXT("LastSightStrength"), Stimulus.Strength);
            BB->SetValueAsFloat(TEXT("LastSightTime"), CurrentTime);
        }
    }
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            const float Score = Stimulus.Strength * 10.f;
            const float Old = BB->GetValueAsFloat(TEXT("HearingSum"));
            BB->SetValueAsFloat(TEXT("HearingSum"), Old + Score);
            BB->SetValueAsFloat(TEXT("LastHearingTime"), CurrentTime);
        }
    }
}

/* TODO: Not working. */
inline void AAIBaseController::SetPerceptionByCharacterAttributeSet(APawn* InPawn)
{
    AAIBaseCharacter* AIChar = Cast<AAIBaseCharacter>(InPawn);
    if (AIChar && AIChar->GetAbilitySystemComponent())
    {
        UAIBaseAttributeSet* AttrSet = Cast<UAIBaseAttributeSet>(AIChar->GetAIAttributeSet());
        if (AttrSet)
        {
            if (SightConfig)
            { 
                SightConfig->SightRadius = AttrSet->GetSightRadius();
                SightConfig->LoseSightRadius = AttrSet->GetLoseSightRadius();
                SightConfig->PeripheralVisionAngleDegrees = AttrSet->GetSightAngle();
                SightConfig->SetMaxAge(AttrSet->GetSightAge());
            }
            if (HearingConfig)
            {
                HearingConfig->HearingRange = AttrSet->GetHearingRange();
                HearingConfig->SetMaxAge(AttrSet->GetHearingAge());
            }
        }
    }
}


