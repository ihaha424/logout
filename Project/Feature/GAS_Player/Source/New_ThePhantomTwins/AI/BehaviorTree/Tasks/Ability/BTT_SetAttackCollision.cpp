// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetAttackCollision.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"

#include "Log/TPTLog.h"

UBTT_SetAttackCollision::UBTT_SetAttackCollision()
{
	NodeName = TEXT("SetAttackCollision");
}

EBTNodeResult::Type UBTT_SetAttackCollision::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    NULLCHECK_RETURN_LOG(Pawn, AILog, Warning, EBTNodeResult::Failed);

    AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(Pawn);
    NULLCHECK_RETURN_LOG(AIBaseCharacter, AILog, Warning, EBTNodeResult::Failed);

    AIBaseCharacter->SetAttackCollision(SetValue);

    return EBTNodeResult::Succeeded;
}

FString UBTT_SetAttackCollision::GetStaticDescription() const
{
	return FString("SetAttackCollision by AIBaseCharacter");
}
