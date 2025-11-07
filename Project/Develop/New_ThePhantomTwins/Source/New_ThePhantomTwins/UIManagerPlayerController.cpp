// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerPlayerController.h"

void AUIManagerPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!UIManager && IsLocalController())
	{
		UIManager = NewObject<UUIManager>(this);
		UIManager->Initialize(this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//									UI Manager											//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////

void AUIManagerPlayerController::SetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (nullptr != UIManager && IsLocalController())
		NetSetWidget(UIKey, bActive, TargetType);
}

UUserWidget* AUIManagerPlayerController::GetWidget(const FString& UIKey) const
{
	UUserWidget* Widget = nullptr;

	if (nullptr != UIManager)
	{
		Widget = UIManager->GetWidget(UIKey);
	}

	return Widget;
}

bool AUIManagerPlayerController::IsRegisterWidget(const FString& UIKey) const
{
	UUserWidget* Widget = nullptr;

	if (nullptr != UIManager)
	{
		Widget = UIManager->GetWidget(UIKey);
	}

	return (nullptr != Widget ? true : false);
}

void AUIManagerPlayerController::RegisterWidget(const FString& Key, UUserWidget* Widget, int32 Order)
{
	if (IsLocalController())
		UIManager->RegisterUI(Key, Widget, Order);
	else
		UE_LOG(LogUIManager, Error, TEXT("AUIManagerPlayerController::RegisterWidget: Player Controller is not LocalController."));
}

void AUIManagerPlayerController::UnregisterWidget(const FString& Key, UUserWidget* Widget)
{
	if (IsLocalController())
		UIManager->UnregisterUI(Key, Widget);
	else
		UE_LOG(LogUIManager, Error, TEXT("AUIManagerPlayerController::UnregisterWidget: Player Controller is not LocalController."));
}



void AUIManagerPlayerController::NetSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (HasAuthority())
		HandleServerSetWidget(UIKey, bActive, TargetType);
	else
		HandleClientSetWidget(UIKey, bActive, TargetType);
}
inline void AUIManagerPlayerController::HandleServerSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
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
			if (AUIManagerPlayerController* PC = Cast<AUIManagerPlayerController>(It->Get()))
			{
				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	case EMessageTargetType::OnlyHost:
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			AUIManagerPlayerController* DefaultPC = Cast<AUIManagerPlayerController>(PC);
			DefaultPC->SetWidget(UIKey, bActive, EMessageTargetType::LocalClient);
		}
		break;

	case EMessageTargetType::AllExceptSelf:
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AUIManagerPlayerController* PC = Cast<AUIManagerPlayerController>(It->Get()))
			{
				if (PC == this)
					continue;

				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("AUIManagerPlayerController::HandleServerSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
inline void AUIManagerPlayerController::HandleClientSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	switch (TargetType)
	{
	case EMessageTargetType::LocalClient:
		if (IsLocalController())
			UIManager->SetWidget(UIKey, bActive);
		else
		{
			UE_LOG(LogUIManager, Error, TEXT("AUIManagerPlayerController::SetWidget: Key: %s"), *UIKey);
			UE_LOG(LogUIManager, Error, TEXT("Invalid access. Client accesses the UI of the host."));
		}
		break;

	case EMessageTargetType::Multicast:
	case EMessageTargetType::OnlyHost:
	case EMessageTargetType::AllExceptSelf:
		C2S_ShowUI(UIKey, bActive, TargetType);
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("AUIManagerPlayerController::HandleClientSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
void AUIManagerPlayerController::C2S_ShowUI_Implementation(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	NetSetWidget(UIKey, bActive, TargetType);
}
void AUIManagerPlayerController::S2C_ShowUI_Implementation(const FString& UIKey, bool bActive)
{
	UIManager->SetWidget(UIKey, bActive);
}
