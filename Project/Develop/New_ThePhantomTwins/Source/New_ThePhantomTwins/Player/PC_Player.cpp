// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Player.h"

#include "EnhancedInputSubsystems.h"
#include "Log/TPTLog.h"
#include "UI/HUD/HUD_PhantomTwins.h"

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

void APC_Player::C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor)
{
	if (HasAuthority())
		Actor->SetOwner(thisPC);
}

//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//									UI Manager											//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////

void APC_Player::SetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (nullptr != UIManager && IsLocalController())
		NetSetWidget(UIKey, bActive, TargetType);
}

UUserWidget* APC_Player::GetWidget(const FString& UIKey) const
{
	UUserWidget* Widget = nullptr;

	if (nullptr != UIManager)
	{
		Widget = UIManager->GetWidget(UIKey);
	}

	return Widget;
}

void APC_Player::RegisterWidget(const FString& Key, UUserWidget* Widget, int32 Order)
{
	if (IsLocalController())
		UIManager->RegisterUI(Key, Widget, Order);
	else
		UE_LOG(LogUIManager, Error, TEXT("APC_Player::RegisterWidget: Player Controller is not LocalController."));
}

void APC_Player::UnregisterWidget(const FString& Key, UUserWidget* Widget)
{
	if (IsLocalController())
		UIManager->RegisterUI(Key, Widget);
	else
		UE_LOG(LogUIManager, Error, TEXT("APC_Player::UnregisterWidget: Player Controller is not LocalController."));
}



void APC_Player::NetSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (HasAuthority())
		HandleServerSetWidget(UIKey, bActive, TargetType);
	else
		HandleClientSetWidget(UIKey, bActive, TargetType);
}
inline void APC_Player::HandleServerSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	switch (TargetType)
	{
	case EMessageTargetType::LocalClient:
		if (IsLocalController())
			UIManager->SetWidget(UIKey, bActive);
		else
			S2C_ShowUI(UIKey, bActive);
		break;

	case EMessageTargetType::Multicast:
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APC_Player* PC = Cast<APC_Player>(It->Get()))
			{
				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	case EMessageTargetType::OnlyHost:
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			APC_Player* DefaultPC = Cast<APC_Player>(PC);
			DefaultPC->SetWidget(UIKey, bActive, EMessageTargetType::LocalClient);
		}
		break;

	case EMessageTargetType::AllExceptSelf:
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APC_Player* PC = Cast<APC_Player>(It->Get()))
			{
				if (PC == this)
					continue;

				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("APC_Player::HandleServerSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
inline void APC_Player::HandleClientSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	switch (TargetType)
	{
	case EMessageTargetType::LocalClient:
		if (IsLocalController())
			UIManager->SetWidget(UIKey, bActive);
		else
		{
			UE_LOG(LogUIManager, Error, TEXT("APC_Player::SetWidget: Key: %s"), *UIKey);
			UE_LOG(LogUIManager, Error, TEXT("Invalid access. Client accesses the UI of the host."));
		}
		break;

	case EMessageTargetType::Multicast:
	case EMessageTargetType::OnlyHost:
	case EMessageTargetType::AllExceptSelf:
		C2S_ShowUI(UIKey, bActive, TargetType);
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("APC_Player::HandleClientSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
void APC_Player::C2S_ShowUI_Implementation(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	NetSetWidget(UIKey, bActive, TargetType);
}
void APC_Player::S2C_ShowUI_Implementation(const FString& UIKey, bool bActive)
{
	UIManager->SetWidget(UIKey, bActive);
}