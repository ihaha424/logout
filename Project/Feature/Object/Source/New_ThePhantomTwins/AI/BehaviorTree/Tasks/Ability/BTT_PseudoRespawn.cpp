// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PseudoRespawn.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "AI/AIEventReceiver.h"

#include "Log/TPTLog.h"

UBTT_PseudoRespawn::UBTT_PseudoRespawn()
{
    NodeName = TEXT("PseudoRespawn");
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_PseudoRespawn::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    NULLCHECK_RETURN_LOG(AICon, AILog, Warning, EBTNodeResult::Failed);
    ACharacter* Character = AICon ? Cast<ACharacter>(AICon->GetPawn()) : nullptr;
    NULLCHECK_RETURN_LOG(Character, AILog, Warning, EBTNodeResult::Failed);
    
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, EBTNodeResult::Failed);

    FVector RespawnLocation = BB->GetValueAsVector(RespawnLocationKey.SelectedKeyName);
    Character->SetActorEnableCollision(false);
    UAIPerceptionComponent* Perception = AICon->GetPerceptionComponent();
    Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
    Perception->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);

    Perception->RequestStimuliListenerUpdate();
    Perception->ForgetAll();

    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(
        this, &UBTT_PseudoRespawn::CompleteRespawn, Character, &OwnerComp, RespawnLocation, Perception);

    Character->GetWorld()->GetTimerManager().SetTimer(
        RespawnTimerHandle,
        RespawnDelegate,
        RespawnDelay,
        false
    );

    return EBTNodeResult::InProgress;
}

FString UBTT_PseudoRespawn::GetStaticDescription() const
{
    return FString("PseudoRespawn");
}

void UBTT_PseudoRespawn::CompleteRespawn(ACharacter* Character, UBehaviorTreeComponent* OwnerComp, FVector Location, UAIPerceptionComponent* Perception)
{
    NULLCHECK_RETURN_LOG(Character, AILog, Warning, );
    NULLCHECK_RETURN_LOG(OwnerComp, AILog, Warning, );

    Character->SetActorEnableCollision(true);
    Character->SetActorLocation(Location);
    Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
    Perception->SetSenseEnabled(UAISense_Hearing::StaticClass(), true);
    Perception->RequestStimuliListenerUpdate();
    if (Character->GetClass()->ImplementsInterface(UAIEventReceiver::StaticClass()))
    {
        IAIEventReceiver::Execute_ApplyRespawn(Character);
    }

    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTT_PseudoRespawn::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (UWorld* W = OwnerComp.GetWorld())
        W->GetTimerManager().ClearTimer(RespawnTimerHandle);
    return EBTNodeResult::Aborted;
}

void UBTT_PseudoRespawn::OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp)
{
    if (UWorld* W = OwnerComp.GetWorld())
        W->GetTimerManager().ClearTimer(RespawnTimerHandle);
    Super::OnInstanceDestroyed(OwnerComp);
}
