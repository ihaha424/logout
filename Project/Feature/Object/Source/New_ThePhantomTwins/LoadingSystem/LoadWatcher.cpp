// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingSystem/LoadWatcher.h"
#include "LoadingSystem/LoadingPrepSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"


void ALoadWatcher::BeginPlay()
{
    Super::BeginPlay();
    // 살짝 지연 후 1회만 시작
    GetWorldTimerManager().SetTimerForNextTick(this, &ALoadWatcher::StartPreloadOnce);
}

void ALoadWatcher::StartPreloadOnce()
{
    UWorld* W = GetWorld();
    if (!W) return;

    if (!bPreloadStarted)
    {
        if (UGameInstance* GI = UGameplayStatics::GetGameInstance(W))
        {
            if (auto* Prep = GI->GetSubsystem<ULoadingPrepSubsystem>())
            {
                Prep->BeginPreload();     // ★ 단 한 번
                bPreloadStarted = true;
            }
        }
    }

    // 폴링 시작 (반복)
    GetWorldTimerManager().SetTimer(PollHandle, this, &ALoadWatcher::PollPreload, 0.05f, true, 0.05f);
}

void ALoadWatcher::PollPreload()
{
    UWorld* W = GetWorld();
    if (!W) 
        return;

    // 완료 체크
    if (UGameInstance* GI2 = UGameplayStatics::GetGameInstance(W))
    {
        if (auto* Prep = GI2->GetSubsystem<ULoadingPrepSubsystem>())
        {
            if (Prep->IsAllLoaded())
            {
                if (APlayerController* PC = UGameplayStatics::GetPlayerController(W, 0))
                {
                    /* if (auto* GM = Cast<ALoadingGameMode>(UGameplayStatics::GetGameMode(W)))
                     {
                         GM->Server_NotifyClientReady(PC);
                     }*/
                }
                // 타이머 정지(더 이상 폴링 불필요)
                GetWorldTimerManager().ClearTimer(PollHandle);
            }
        }
    }
}
