// Fill out your copyright notice in the Description page of Project Settings.


#include "W_MainMenu.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../IPEncryption.h"


void UW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

}

void UW_MainMenu::StartLobby(const FName LevelName)
{
    FString IP = UIPEncryption::GetLocalIPAddress();
    FString HexString = UIPEncryption::IP2Code(IP);
    UE_LOG(LogTemp, Error, TEXT("Sever: %s"), *HexString);


	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, TEXT("listen"));
}

void UW_MainMenu::ConnectLobby(const FName LevelName)
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FString Code = LevelName.ToString();
        FString IP = UIPEncryption::Code2IP(Code);

        UE_LOG(LogTemp, Error, TEXT("Client: %s"), *IP);

        PC->ClientTravel(IP, ETravelType::TRAVEL_Absolute);
    }
}

