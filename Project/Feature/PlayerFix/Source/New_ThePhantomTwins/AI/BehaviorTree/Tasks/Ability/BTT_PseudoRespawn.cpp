// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PseudoRespawn.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

#include "Log/TPTLog.h"

UBTT_PseudoRespawn::UBTT_PseudoRespawn()
{
    NodeName = TEXT("PseudoRespawn");
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
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(
        this, &UBTT_PseudoRespawn::CompleteRespawn, Character, &OwnerComp, RespawnLocation);

    FTimerHandle RespawnTimerHandle;

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

void UBTT_PseudoRespawn::CompleteRespawn(ACharacter* Character, UBehaviorTreeComponent* OwnerComp, FVector Location)
{
    NULLCHECK_RETURN_LOG(Character, AILog, Warning, );
    NULLCHECK_RETURN_LOG(OwnerComp, AILog, Warning, );

    Character->SetActorEnableCollision(true);
    Character->SetActorLocation(Location);
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
