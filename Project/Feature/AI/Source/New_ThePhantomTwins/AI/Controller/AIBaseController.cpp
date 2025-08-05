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
#include "AbilitySystemGlobals.h"

#include "AI/Utility/AIBaseState.h"
#include "../Character/AIBaseCharacter.h"
#include "../Attributes/AIBaseAttributeSet.h"
#include "../Utility/SplineActor.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

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
    
    
    
                                     // Scanner
    StimulusPriorityMap = {             //StimulusPriorityMap = {
      { "PlayerActor", {1, 1} },             //  { "PlayerActor", 1 },
      { "NoiseItem", {2, 100} },               //  { "EnemyActor", 1 },
      { "PlayerRun", {3, 30} },               //  { "EnemyRun", 2 },
      { "PlayerWalk", {4, 10} },              //  { "EnemyWalk", 3 },
    };                                  //  { "PlayerRun", 4 },
                                        //  { "NoiseItem", 5 },
                                        //  { "PlayerWalk", 6 }
                                        //};
}

void AAIBaseController::BeginPlay()
{
    Super::BeginPlay();
   
}

void AAIBaseController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
    const FName DefaultState = TEXT("AIState");
    BBComponent->SetValueAsEnum(DefaultState, static_cast<uint8>(EAIBaseState::Default));

    SetPerceptionByCharacterAttributeSet(InPawn);

    GetAbilitySystemComponent()->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);
    GetAbilitySystemComponent()->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default);
}

void AAIBaseController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !GetBlackboardComponent()) return;

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
        NULLCHECK_RETURN_LOG(ASC, AILog, Log, );
        // Player
        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player))
        {
            if (Stimulus.WasSuccessfullySensed())
            {
                AddPerceptionSightList(Actor);
            }
            else
            {
                RemovePerceptionSightList(Actor);
            }
        }
        // Object
        else if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Object))
        {
            if (Stimulus.WasSuccessfullySensed())
            {

            }
            else
            {

            }
        }
       
    }
    else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        const int32 Priority = GetStimulusPriority(Stimulus.Tag);
        const float CurrentTime = GetWorld()->GetTimeSeconds();
        if (Stimulus.WasSuccessfullySensed())
        {
            const float Score = GetStimulusStrength(Stimulus.Tag);
            const float Old = BB->GetValueAsFloat(TEXT("HearingSum"));
            BB->SetValueAsFloat(TEXT("Priority"), Priority);
            BB->SetValueAsFloat(TEXT("HearingSum"), Old + Score);
            BB->SetValueAsFloat(TEXT("LastHearingTime"), CurrentTime);
            BB->SetValueAsVector(TEXT("StimulusLocation"), Stimulus.StimulusLocation);
        }
        else
        {

        }
    }
    
}

void AAIBaseController::AddPerceptionSightList(AActor* Actor)
{
    if(INDEX_NONE == PerceptionSightList.Find(Actor))
        PerceptionSightList.Add(Actor);

    if (!SightTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(
            SightTimerHandle,
            this,
            &AAIBaseController::FindCloseActor,
            0.2f,
            true
        );
    }
    FindCloseActor();
}

void AAIBaseController::RemovePerceptionSightList(AActor* Actor)
{
    PerceptionSightList.Remove(Actor);
    if (PerceptionSightList.Num() < 1 && SightTimerHandle.IsValid())
        GetWorld()->GetTimerManager().ClearTimer(SightTimerHandle);
}

void AAIBaseController::FindCloseActor()
{
    AActor* OwnerActor = GetPawn();
    AActor* ClosestActor = nullptr;
    float ClosestDistanceSq = FLT_MAX;
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    UBlackboardComponent* BB = GetBlackboardComponent();
    for (AActor* Target : PerceptionSightList)
    {
        if (!IsValid(Target) || Target == OwnerActor)
            continue;
        UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed))
            continue;
        float DistSq = OwnerActor->GetDistanceTo(Target);

        if (DistSq < ClosestDistanceSq)
        {
            ClosestDistanceSq = DistSq;
            ClosestActor = Target;
        }
    }

    if (nullptr != ClosestActor)
    {
        bool HasSight = false;
        if (const FActorPerceptionInfo* Info = PerceptionComponent->GetActorInfo(*ClosestActor))
        {
            for (const FAIStimulus& Stimulus : Info->LastSensedStimuli)
            {
                if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>() && Stimulus.WasSuccessfullySensed())
                {
                    HasSight = true;
                    BB->SetValueAsVector(TEXT("StimulusLocation"), Stimulus.StimulusLocation);
                    BB->SetValueAsFloat(TEXT("LastSightTime"), CurrentTime);
                    
                    AActor* CurTargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
                    if (CurTargetActor != ClosestActor)
                    {
                        BB->SetValueAsObject(TEXT("TargetActor"), ClosestActor);

                        AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(GetPawn());
                        AIBaseCharacter->ExcuteChaseActorGA(ClosestActor);
                    }
                }
            }
        }
        
        if (!HasSight)
        {
            BB->SetValueAsVector(TEXT("StimulusLocation"), ClosestActor->GetActorLocation());
            BB->SetValueAsFloat(TEXT("LastSightTime"), CurrentTime);

            AActor* CurTargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
            if (CurTargetActor != ClosestActor)
            {
                BB->SetValueAsObject(TEXT("TargetActor"), ClosestActor);

                AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(GetPawn());
                AIBaseCharacter->ExcuteChaseActorGA(ClosestActor);
            }
        }
    }
}

int32 AAIBaseController::GetStimulusPriority(const FName& Tag)
{
    return StimulusPriorityMap.Contains(Tag) ? StimulusPriorityMap[Tag].X : std::numeric_limits<int32>::max();
}

int32 AAIBaseController::GetStimulusStrength(const FName& Tag)
{
    return StimulusPriorityMap.Contains(Tag) ? StimulusPriorityMap[Tag].Y : std::numeric_limits<int32>::max();
}

UAbilitySystemComponent* AAIBaseController::GetAbilitySystemComponent() const
{
    return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
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


