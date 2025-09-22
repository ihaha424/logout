// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Endding.h"


void AGM_Endding::TravelToLevel(const FName LevelName, bool bAbsolute)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC->HasAuthority()) return;

    FString LevelPathWithListen = LevelName.ToString() + TEXT("?listen");

    UE_LOG(LogTemp, Log, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
    GetWorld()->ServerTravel(LevelPathWithListen, bAbsolute);
}
