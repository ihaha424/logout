// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PseudoRespawn.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

UBTT_PseudoRespawn::UBTT_PseudoRespawn()
{
    NodeName = TEXT("Pseudo Respawn");

    RespawnLocation = FVector::ZeroVector;
    RespawnDelay = 5.0f;
}

EBTNodeResult::Type UBTT_PseudoRespawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    ACharacter* Character = AICon ? Cast<ACharacter>(AICon->GetPawn()) : nullptr;

    if (!Character)
    {
        return EBTNodeResult::Failed;
    }

    // 1. ÀÌµ¿ ¹× ¼û±è
    Character->SetActorLocation(RespawnLocation);
    Character->SetActorHiddenInGame(true);
    Character->SetActorEnableCollision(false);

    // 2. µô·¹ÀÌ ÈÄ ÀçÈ°¼ºÈ­ ¿¹¾à
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(
        this, &UBTT_PseudoRespawn::CompleteRespawn, Character, &OwnerComp);

    FTimerHandle RespawnTimerHandle;

    Character->GetWorld()->GetTimerManager().SetTimer(
        RespawnTimerHandle,
        RespawnDelegate,
        RespawnDelay,
        false
    );

    return EBTNodeResult::InProgress;
}

void UBTT_PseudoRespawn::CompleteRespawn(ACharacter* Character, UBehaviorTreeComponent* OwnerComp)
{
    if (!Character || !OwnerComp)
    {
        return;
    }

    Character->SetActorHiddenInGame(false);
    Character->SetActorEnableCollision(true);

    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}