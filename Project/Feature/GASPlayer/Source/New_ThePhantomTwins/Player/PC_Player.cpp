// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Player.h"

void APC_Player::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
