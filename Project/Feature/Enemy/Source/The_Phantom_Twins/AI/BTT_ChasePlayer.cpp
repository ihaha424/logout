// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChasePlayer.h"

UBTT_ChasePlayer::UBTT_ChasePlayer()
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{


	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
