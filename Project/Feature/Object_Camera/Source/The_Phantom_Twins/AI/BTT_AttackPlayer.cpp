// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AttackPlayer.h"

UBTT_AttackPlayer::UBTT_AttackPlayer()
{
	NodeName = TEXT("Attack Player");
}

EBTNodeResult::Type UBTT_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Success Attack Player"));

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
