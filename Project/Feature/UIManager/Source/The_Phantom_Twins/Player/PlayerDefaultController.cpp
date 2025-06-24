// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDefaultController.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerDefaultController::APlayerDefaultController()
{

}

void APlayerDefaultController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!UIManager && IsLocalController())
	{
		UIManager = NewObject<UUIManager>(this);
		UIManager->Initialize(this);
	}
}

void APlayerDefaultController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void APlayerDefaultController::C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor)
{
	if(HasAuthority())
		Actor->SetOwner(thisPC);
}

//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//									UI Manager											//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////

void APlayerDefaultController::SetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (nullptr != UIManager && IsLocalController())
		NetSetWidget(UIKey, bActive, TargetType);
}

void APlayerDefaultController::RegisterWidget(const FString& Key, UUserWidget* Widget, int32 Order)
{
	if (IsLocalController())
		UIManager->RegisterUI(Key, Widget, Order);
	else
		UE_LOG(LogUIManager, Error, TEXT("APlayerDefaultController::RegisterWidget: Player Controller is not LocalController."));
}

void APlayerDefaultController::UnregisterWidget(const FString& Key, UUserWidget* Widget)
{
	if (IsLocalController())
		UIManager->RegisterUI(Key, Widget);
	else
		UE_LOG(LogUIManager, Error, TEXT("APlayerDefaultController::UnregisterWidget: Player Controller is not LocalController."));
}



void APlayerDefaultController::NetSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	if (HasAuthority())
		HandleServerSetWidget(UIKey, bActive, TargetType);
	else
		HandleClientSetWidget(UIKey, bActive, TargetType);
}
inline void APlayerDefaultController::HandleServerSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
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
			if (APlayerDefaultController* PC = Cast<APlayerDefaultController>(It->Get()))
			{
				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	case EMessageTargetType::OnlyHost:
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			APlayerDefaultController* DefaultPC = Cast<APlayerDefaultController>(PC);
			DefaultPC->SetWidget(UIKey, bActive, EMessageTargetType::LocalClient);
		}
		break;

	case EMessageTargetType::AllExceptSelf:
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerDefaultController* PC = Cast<APlayerDefaultController>(It->Get()))
			{
				if (PC == this)
					continue;

				PC->S2C_ShowUI(UIKey, bActive);
			}
		}
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("APlayerDefaultController::HandleServerSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
inline void APlayerDefaultController::HandleClientSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	switch (TargetType)
	{
	case EMessageTargetType::LocalClient:
		if (IsLocalController())
			UIManager->SetWidget(UIKey, bActive);
		else
		{
			UE_LOG(LogUIManager, Error, TEXT("APlayerDefaultController::SetWidget: Key: %s"), *UIKey);
			UE_LOG(LogUIManager, Error, TEXT("Invalid access. Client accesses the UI of the host."));
		}
		break;

	case EMessageTargetType::Multicast:
	case EMessageTargetType::OnlyHost:
	case EMessageTargetType::AllExceptSelf:
		C2S_ShowUI(UIKey, bActive, TargetType);
		break;

	default:
		UE_LOG(LogUIManager, Error, TEXT("APlayerDefaultController::HandleClientSetWidget: TargetType is Invaild."));
		ensureMsgf(false, TEXT("Invalid EMessageTargetType passed to SetWidget"));
		break;
	}
}
void APlayerDefaultController::C2S_ShowUI_Implementation(const FString& UIKey, bool bActive, EMessageTargetType TargetType)
{
	NetSetWidget(UIKey, bActive, TargetType);
}
void APlayerDefaultController::S2C_ShowUI_Implementation(const FString& UIKey, bool bActive)
{
	UIManager->SetWidget(UIKey, bActive);
}

