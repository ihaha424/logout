// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_PhantomTwins.h"
#include "GS_PhantomTwins.h"
#include "AI/Utility/BossSpawner.h"
#include "Blueprint/UserWidget.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"


#include "Log/TPTLog.h"

void AGM_PhantomTwins::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AGM_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority()) return;

    if (TimeLimitSeconds > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            TimerHandle_SpawnByTime, this, &AGM_PhantomTwins::OnTimeLimitReached,
            TimeLimitSeconds, false);
    }

    if (AGS_PhantomTwins* GS = GetGameState<AGS_PhantomTwins>())
    {
        UTPTSaveGame* TPTLocalPlayerSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
        GS->SetMapData(TPTLocalPlayerSaveGame->IdentifyMapData.MapType);

        ItemChangedHandle = GS->OnCollectedItemCountChanged().AddUObject(
            this, &AGM_PhantomTwins::OnItemCountChanged);

        if (GS->CoreCount >= RequiredItemCount)
        {
            RequestBossSpawn();
        }
    }
}

void AGM_PhantomTwins::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    TotalPlayerCount++;
}

void AGM_PhantomTwins::NotifyPlayerDied(bool isDead)
{
    if (isDead)
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
    //S2A_ShowFadeUI();

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            FString MapName = GetWorld()->GetOutermost()->GetName();
            FString LevelPathWithListen = MapName + TEXT("?listen");
            GetWorld()->ServerTravel(LevelPathWithListen, false);
        }, Delay, false);
}

void AGM_PhantomTwins::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (HasAuthority())
    {
        AGS_PhantomTwins* GS = GetGameState<AGS_PhantomTwins>();
        if (GS)
        {
            if (ItemChangedHandle.IsValid())
                GS->OnCollectedItemCountChanged().Remove(ItemChangedHandle);
        }
        GetWorldTimerManager().ClearTimer(TimerHandle_SpawnByTime);
    }
    Super::EndPlay(EndPlayReason);
}

void AGM_PhantomTwins::SeverToLevel(const FName LevelName, bool bAbsolute)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC->HasAuthority()) return;

    FString LevelPathWithListen = LevelName.ToString() + TEXT("?listen");

    TPT_LOG(GameRuleLog, Log, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
    GetWorld()->ServerTravel(LevelPathWithListen, bAbsolute);
}

void AGM_PhantomTwins::OnItemCountChanged(int32 NewCount)
{
    if (NewCount >= RequiredItemCount)
    {
        RequestBossSpawn();
    }
}

void AGM_PhantomTwins::OnTimeLimitReached()
{
    RequestBossSpawn();
}

void AGM_PhantomTwins::RequestBossSpawn()
{
    AGS_PhantomTwins* GS = GetGameState<AGS_PhantomTwins>();
    if (!GS || GS->bBossSpawned) return;

    if (PreferredSpawnActor.IsValid())
    {
        PreferredSpawnActor.Get()->SpawnBossOnce();
        if (GS->bBossSpawned) return;
    }
    else
    {
        TPT_LOG(GameRuleLog, Error, TEXT("PreferredSpawnActor is Invalid."));
    }
}
