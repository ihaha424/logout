// Fill out your copyright notice in the Description page of Project Settings.


#include "W_MainMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

}

void UW_MainMenu::StartLobby(const FName LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, TEXT("listen"));
}

void UW_MainMenu::ConnectLobby(const FName LevelName)
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FString URL = LevelName.ToString();
        PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
    }
}
