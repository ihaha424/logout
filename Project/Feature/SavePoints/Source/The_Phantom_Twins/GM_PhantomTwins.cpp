// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_PhantomTwins.h"

#include "PhantomTwinsInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "OutGame/SaveGame/TPTSaveGameData.h"


#include "Kismet/KismetSystemLibrary.h"


void AGM_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();
    UPhantomTwinsInstance* GameInstance = GetGameInstance<UPhantomTwinsInstance>();
    if (GameInstance && GameInstance->bIsRestartingLevel)
    {
        bShouldOverrideSpawns = true;
        OverrideSpawnDatas = GameInstance->RestartPlayerDatas;

        // Listen Server 환경에서 서버 플레이어(호스트)만 강제 리스폰
        //for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        //{
        //    APlayerController* PC = Cast<APlayerController>(It->Get());
        //    if (PC && PC->IsLocalController() && GetNetMode() == NM_ListenServer)
        //    {
        //        // 기존 Pawn 제거
        //        if (APawn* OldPawn = PC->GetPawn())
        //        {
        //            OldPawn->Destroy();
        //        }

        //        // 서버 플레이어의 PlayerID는 "Host"로 강제
        //        FString PlayerID = TEXT("Host");
        //        const FPlayerSaveData* FoundData = OverrideSpawnDatas.FindByPredicate(
        //            [&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });

        //        if (FoundData)
        //        {
        //            // 원하는 Pawn 클래스로 Spawn (DefaultPawnClass 등)
        //            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
        //                DefaultPawnClass, // 프로젝트에 맞는 PawnClass로 교체
        //                FoundData->PlayerLocation,
        //                FoundData->PlayerRotation
        //            );
        //            if (NewPawn)
        //            {
        //                PC->Possess(NewPawn);
        //                UE_LOG(LogTemp, Warning, TEXT("[SERVER RESPAWN] Host player respawned at %s"), *FoundData->PlayerLocation.ToString());
        //            }
        //        }
        //        else
        //        {
        //            UE_LOG(LogTemp, Warning, TEXT("[SERVER RESPAWN] Host player: No matching save data!"));
        //        }
        //    }
        //}

        // 플래그를 반드시 다시 꺼줘야 다음 맵 로드에 영향을 주지 않습니다.
        GameInstance->bIsRestartingLevel = false;
    }
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

    // GameInstance에 현재 플레이어 위치 정보를 저장
    UPhantomTwinsInstance* GameInstance = GetGameInstance<UPhantomTwinsInstance>();
    if (GameInstance)
    {
        // 1. 재시작 플래그를 켜고, 이전 데이터를 비웁니다.
        GameInstance->bIsRestartingLevel = true;
        GameInstance->RestartPlayerDatas.Empty();

        // 2. 현재 모든 플레이어를 순회하며 위치를 저장합니다.
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            AController* Controller = It->Get();
            if (Controller && Controller->PlayerState && Controller->GetPawn())
            {
                FPlayerSaveData RestartData;

                FString PlayerID;
                // 서버 플레이어(호스트)라면 무조건 "Host"로 저장
                if (Controller->IsLocalController() && GetNetMode() == NM_ListenServer)
                    PlayerID = TEXT("Host");
                else if (Controller->PlayerState->GetUniqueId().IsValid())
                    PlayerID = Controller->PlayerState->GetUniqueId().ToString();
                else
                    PlayerID = Controller->PlayerState->GetPlayerName();

                RestartData.PlayerID = PlayerID;
                RestartData.PlayerLocation = Controller->GetPawn()->GetActorLocation();
                RestartData.PlayerRotation = Controller->GetPawn()->GetActorRotation();

                // 로그로 확인
                UE_LOG(LogTemp, Warning, TEXT("[SAVE] PlayerID: %s, Location: %s"), *RestartData.PlayerID, *RestartData.PlayerLocation.ToString());

                GameInstance->RestartPlayerDatas.Add(RestartData);
            }
        }

    }

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            FString MapName = GetWorld()->GetOutermost()->GetName();
            FString LevelPathWithListen = MapName + TEXT("?listen");
            GetWorld()->ServerTravel(LevelPathWithListen, false);
        }, Delay, false);
}

AActor* AGM_PhantomTwins::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
    UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("GM ptr: %p"), this), true, true, FLinearColor::Red, 5.0f);
    if (bShouldOverrideSpawns && Player)
    {
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("FindPlayerStart_Implementation: [FindPlayerStart_Implementation] Player: %s, IncomingName: %s"), *Player->GetName(), *IncomingName), true, true, FLinearColor::Red, 5.0f);
        UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("FindPlayerStart_Implementation: [2222222222222222222222222222] bShouldOverrideSpawns is %d or %f is null"), bShouldOverrideSpawns, Player), true, true, FLinearColor::Red, 5.0f);

        FString PlayerId;
        if (Player->PlayerState && Player->PlayerState->GetUniqueId().IsValid())
            PlayerId = Player->PlayerState->GetUniqueId().ToString();
        else if (Player->IsLocalController() && GetNetMode() == NM_ListenServer)
            PlayerId = TEXT("Host");
        else
            PlayerId = TEXT("Unknown");

        UE_LOG(LogTemp, Warning, TEXT("[RESPAWN] PlayerID: %s"), *PlayerId);

        const FPlayerSaveData* FoundData = OverrideSpawnDatas.FindByPredicate(
            [&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerId; });

        if (FoundData)
        {
            UE_LOG(LogTemp, Warning, TEXT("[RESPAWN] FoundData Location: %s"), *FoundData->PlayerLocation.ToString());
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            APlayerStart* TempPlayerStart = GetWorld()->SpawnActor<APlayerStart>(
                FoundData->PlayerLocation, FoundData->PlayerRotation, SpawnParams);

            UE_LOG(LogTemp, Log, TEXT("Player %s is restarting at saved location."), *PlayerId);
            return TempPlayerStart;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[RESPAWN] No matching data found for PlayerID: %s!"), *PlayerId);
        }
    }
    else
    {
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("FindPlayerStart_Implementation: [FindPlayerStart_Implementation] bShouldOverrideSpawns is %d or %f is null"), bShouldOverrideSpawns, Player), true, true, FLinearColor::Red, 5.0f);
        if (!Player)
			UE_LOG(LogTemp, Warning, TEXT("[FindPlayerStart_Implementation] Player3333333333333333333333333333333333333333333333333333"));
    	UE_LOG(LogTemp, Warning, TEXT("[FindPlayerStart_Implementation] Fail!"));
    }

    return Super::FindPlayerStart_Implementation(Player, IncomingName);
}