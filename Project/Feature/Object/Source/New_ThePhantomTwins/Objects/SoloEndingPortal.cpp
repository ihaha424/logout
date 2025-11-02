// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloEndingPortal.h"
#include "player/PC_Player.h"
#include "player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Log/TPTLog.h"

ASoloEndingPortal::ASoloEndingPortal() : AInteractableObject()
{
}

void ASoloEndingPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ASoloEndingPortal::OnInteractClient_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(false);

	APC_Player* PC_Player = Cast<APC_Player>(Interactor->GetController());
	NULLCHECK_RETURN_LOG(PC_Player, ObjectLog, Error, );
	PC_Player->SetWidget(TEXT("SoloLogout"), true, EMessageTargetType::LocalClient);

	// 마우스 커서 표시
	PC_Player->bShowMouseCursor = true;

	// 입력 모드를 UI Only로 설정
	FInputModeUIOnly InputMode;
	PC_Player->SetInputMode(InputMode);
}

void ASoloEndingPortal::ShowSoloPortalActor(bool bVisible)
{
	if (bVisible)
	{
		SetActorHiddenInGame(false);
		//SetActorEnableCollision(true);
	}
	else
	{
		SetActorHiddenInGame(true);
		//SetActorEnableCollision(false);
	}
}

void ASoloEndingPortal::SetSoloPortalCollision(bool bActived)
{
	if (bActived)
	{
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

