// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_PhantomTwins.h"
#include "GS_PhantomTwins.h"
#include "AI/Utility/BossSpawner.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"


#include "Log/TPTLog.h"
#include "Player/PC_Player.h"

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
        if(TPTLocalPlayerSaveGame->IdentifyMapData.MapType == EMapType::None)
            GS->SetMapData(EMapType::ST2);
        else
            GS->SetMapData(TPTLocalPlayerSaveGame->IdentifyMapData.MapType);

        ItemChangedHandle = GS->OnCollectedItemCountChanged().AddUObject(
            this, &AGM_PhantomTwins::OnItemCountChanged);

        if (GS->CoreCount >= RequiredItemCount)
        {
            RequestBossSpawn();
        }
        GS->OnClickedRestartChanged.AddDynamic(this, &ThisClass::NotifyPlayerClickRestart);
        GS->OnClickedGameStopChanged.AddDynamic(this, &ThisClass::NotifyPlayerClickedGameStop);
    	GS->OnClickedAgreeWithGameStopChanged.AddDynamic(this, &ThisClass::NotifyPlayerAgreeWithGameStop);
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
        ShowGameOverUI();
    }
}

void AGM_PhantomTwins::NotifyPlayerClickRestart(bool bIsHostClicked, bool bIsClientClicked)
{
    if (bIsHostClicked)
        HostClick = 1;
    else
        HostClick = 0;

    if (bIsClientClicked)
        ClientClick = 1;
    else
        ClientClick = 0;

    if (HostClick + ClientClick >= TotalPlayerCount)
    {
        RestartWithDelay(2.f);
    }
}

void AGM_PhantomTwins::ShowGameOverUI()
{
    SetAllPlayerUIMode(true);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player*ServerPC = Cast< APC_Player>(PC);
    ServerPC->SetWidget(TEXT("GameOver"), true, EMessageTargetType::Multicast);
}

void AGM_PhantomTwins::NotifyPlayerClickedGameStop(FName LevelName)
{
    DestinationLevelName = LevelName;
    TPT_LOG(OutGameLog, Error, TEXT("%s"), *DestinationLevelName.ToString());
    ShowGameStopUI();
}

void AGM_PhantomTwins::ShowGameStopUI()
{
    SetAllPlayerUIMode(true);
    // TODO 게임시간을 멈출 다른 방법 찾기
	// 게임 틱 마저 멈추게 되어, UI 동기화가 되지않음.
    UGameplayStatics::SetGamePaused(GetWorld(), true);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("ESC"), false, EMessageTargetType::Multicast);
    ServerPC->SetWidget(TEXT("GameStop"), true, EMessageTargetType::Multicast);
}

void AGM_PhantomTwins::NotifyPlayerAgreeWithGameStop(int32 HostSelect, int32 ClientSelect)
{
    if (HostSelect == 1 && ClientSelect == 1)
    {
        // 그냥  타이머를 넣으니까 멀티캐스트가 안됨.
        ShowLoadingScene();
        SeverToLevel(DestinationLevelName, false);
    }
    else if ((HostSelect != 0 && ClientSelect != 0) && (HostSelect == 2 || ClientSelect == 2))
    {
        
        ShowResumeCountUI();
    }
}

void AGM_PhantomTwins::ShowResumeCountUI()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("GameStop"), false, EMessageTargetType::Multicast);
    ServerPC->SetWidget(TEXT("ResumeCount"), true, EMessageTargetType::Multicast);
}


void AGM_PhantomTwins::ShowLoadingScene()
{
    SetAllPlayerUIMode(true);
    NULLCHECK_RETURN_LOG(GetWorld(), OutGameLog, Error, );
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);
    ServerPC->SetWidget(TEXT("Loading"), true, EMessageTargetType::Multicast);

    SetAllPlayerUIMode(false);
}

void AGM_PhantomTwins::RestartWithDelay(float Delay)
{
    ShowLoadingScene();
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            FString MapName = GetWorld()->GetOutermost()->GetName();
            FString LevelPathWithListen = MapName + TEXT("?listen");
            GetWorld()->ServerTravel(LevelPathWithListen, false);
        }, Delay, false);
}

void AGM_PhantomTwins::ResumePlay()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("ResumeCount"), false, EMessageTargetType::Multicast);
    SetAllPlayerUIMode(false);
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

void AGM_PhantomTwins::Delay(float Time)
{
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]() {},
        Time,
        false
    );
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

void AGM_PhantomTwins::SetAllPlayerUIMode(bool bIsUIMode)
{
    AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
    NULLCHECK_RETURN_LOG(GS, OutGameLog, Error, );

    for (APlayerState* PS : GS->PlayerArray)
    {
        NULLCHECK_RETURN_LOG(PS, OutGameLog, Error, );
        APlayerController* PC = Cast<APlayerController>(PS->GetOwner());
        NULLCHECK_RETURN_LOG(PC, OutGameLog, Error, );
        APC_Player* PLayerPC = Cast<APC_Player>(PC);
        NULLCHECK_RETURN_LOG(PLayerPC, OutGameLog, Error, );

        if (bIsUIMode)
        {
	        FInputModeUIOnly InputModeData;
            PLayerPC->SetInputMode(InputModeData);
            PLayerPC->bShowMouseCursor = true;
        }
        else
        {
            FInputModeGameOnly GameInputMode;
            PLayerPC->SetInputMode(GameInputMode);
            PLayerPC->bShowMouseCursor = false;
        }
    }
}
