// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDefaultState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerBase.h"

APlayerDefaultState::APlayerDefaultState()
{
}

void APlayerDefaultState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerDefaultState, MoveSpeedInfo);
	DOREPLIFETIME(APlayerDefaultState, NoiseInfo);
	DOREPLIFETIME(APlayerDefaultState, bIsGroggy);
}

void APlayerDefaultState::OnRep_Groggy()
{
	if (APlayerBase* OwnerPlayer = Cast<APlayerBase>(GetPawn()))
	{
		OwnerPlayer->SetGroggyWidget(bIsGroggy);
	}
}
