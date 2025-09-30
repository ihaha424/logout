// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Player.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Log/TPTLog.h"
#include "UI/HUD/HUD_PhantomTwins.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PS_Player.h"
#include "CM_LogOut.h"
#include "GS_PhantomTwins.h"
#include "OutGame/HubMap/GS_HubMap.h"

APC_Player::APC_Player()
{
	CheatClass = UCM_LogOut::StaticClass();
}

APC_Player* APC_Player::GetLocalPlayerController(AActor* Actor)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(Actor->GetWorld(), 0);
	NULLCHECK_RETURN_LOG(PC, PlayerLog, Error, nullptr);
	return Cast<APC_Player>(PC);
}

void APC_Player::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!UIManager && IsLocalController())
	{
		UIManager = NewObject<UUIManager>(this);
		UIManager->Initialize(this);
	}
}

void APC_Player::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if (IMC)
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(IMC, 0); // 우선순위는 필요에 따라 조정
			}
		}
	}

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void APC_Player::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APC_Player::TurnOffESC()
{
	SetWidget(TEXT("ESC"), false, EMessageTargetType::LocalClient);
	bShowMouseCursor = false;
	FInputModeGameOnly GameInputData;
	SetInputMode(GameInputData);
}

void APC_Player::C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor)
{
	if (HasAuthority())
		Actor->SetOwner(thisPC);
}

void APC_Player::C2S_ClickedRestart_Implementation(const bool bIsClicked)
{
	if (!HasAuthority())
		return;

	const bool bIsHost = IsLocalController();
	AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_PhantomTwins' Fail"));
		return;
	}
	GS->SetCharacterClickedRestart(bIsClicked, bIsHost);
}

void APC_Player::C2S_ClickedGameStop_Implementation(const FName LevelName)
{
	AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_PhantomTwins' Fail"));
		return;
	}
	GS->SetCharacterClickedGameStop(LevelName);
}

void APC_Player::C2S_ClickedAgreeWithGameStop_Implementation(const int32 SelectBtn)
{
	if (!HasAuthority())
		return;

	const bool bIsHost = IsLocalController();
	AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
	if (!GS)
	{
		TPT_LOG(OutGameLog, Error, TEXT("Cast to 'AGS_PhantomTwins' Fail"));
		return;
	}
	GS->SetCharacterAgreeWithGameStop(SelectBtn, bIsHost);
}
