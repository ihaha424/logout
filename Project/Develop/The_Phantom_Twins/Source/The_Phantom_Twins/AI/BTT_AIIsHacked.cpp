// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AIIsHacked.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

UBTT_AIIsHacked::UBTT_AIIsHacked()
{
	NodeName = TEXT("AI Is Hacked");
}

EBTNodeResult::Type UBTT_AIIsHacked::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("AI Is Hacked Task Executed"));
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    AMyAICharacter* AICharacter = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AICharacter)
    {
        return EBTNodeResult::Failed;
    }
    UE_LOG(LogTemp, Warning, TEXT("AI Is Hacked Task Executed222222222222"));
    // 1. 이동 멈추기
    AICharacter->GetCharacterMovement()->StopMovementImmediately();
    AIController->StopMovement();

    // 3. 5초 후 Task 완료

    FTimerDelegate TimerDel;
    TimerDel.BindLambda([this, &OwnerComp]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        });

    FTimerHandle TimerHandle;
    AIController->GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        TimerDel,
        5.0f,
        false
    );

    return EBTNodeResult::InProgress;
}
