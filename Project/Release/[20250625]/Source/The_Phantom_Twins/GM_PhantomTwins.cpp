// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_PhantomTwins.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void AGM_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();
}

void AGM_PhantomTwins::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    TotalPlayerCount++;
}

void AGM_PhantomTwins::NotifyPlayerDied(AController* DeadPlayer, bool isDead)
{
    if(isDead)
        DeadPlayerCount++;
    else
        DeadPlayerCount--;

    if (DeadPlayerCount >= TotalPlayerCount)
    {
        RestartLevelWithDelay(3.0f);
    }
}

void AGM_PhantomTwins::S2A_ShowFadeUI_Implementation()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (PC->IsLocalController())
            {
                if (FadeUI)
                {
                    FadeUI->AddToViewport();
                }
            }
        }
    }
}

void AGM_PhantomTwins::RestartLevelWithDelay(float Delay)
{
    S2A_ShowFadeUI();

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            FString MapName = GetWorld()->GetOutermost()->GetName();
            FString LevelPathWithListen = MapName + TEXT("?listen");
            GetWorld()->ServerTravel(LevelPathWithListen, false);
        }, Delay, false);
}
