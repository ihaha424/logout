// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"
#include "Net/UnrealNetwork.h"

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, CharacterType);
}
