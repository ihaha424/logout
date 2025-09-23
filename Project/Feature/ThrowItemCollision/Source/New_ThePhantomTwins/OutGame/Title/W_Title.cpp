// Fill out your copyright notice in the Description page of Project Settings.


#include "W_Title.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "../IPEncryption.h"


void UW_Title::NativeConstruct()
{
    Super::NativeConstruct();

}

void UW_Title::StartLobby(const FName LevelName)
{
    UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, TEXT("listen"));
}

void UW_Title::ConnectLobby(const FName LevelName)
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        FString Code = LevelName.ToString();
        FString IP = UIPEncryption::Code2IP(Code);

        PC->ClientTravel(IP, ETravelType::TRAVEL_Absolute);
    }
}

