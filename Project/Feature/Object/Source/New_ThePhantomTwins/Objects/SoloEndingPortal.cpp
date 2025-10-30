// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloEndingPortal.h"
#include "player/PC_Player.h"
#include "player/PlayerCharacter.h"
#include "Log/TPTLog.h"

ASoloEndingPortal::ASoloEndingPortal()
{

}

void ASoloEndingPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ASoloEndingPortal::OnInteractClient_Implementation(const APawn* Interactor)
{
	Super::OnInteractClient_Implementation(Interactor);

	APC_Player* PC_Player = Cast<APC_Player>(Interactor->GetController());
	NULLCHECK_RETURN_LOG(PC_Player, ObjectLog, Error, );
	PC_Player->SetWidget(TEXT("SoloLogout"), true, EMessageTargetType::LocalClient);
}
