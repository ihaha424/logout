// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_PhantomTwins.h"
#include "GS_PhantomTwins.h"
#include "PhantomTwinsInstance.h"
#include "AI/Utility/BossSpawner.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"

#include "Log/TPTLog.h"
#include "Player/PC_Player.h"
#include "SaveGame/TPTSaveGameManager.h"

AGM_PhantomTwins::AGM_PhantomTwins()
{
	PlayerControllerClass = APC_Player::StaticClass();
	GameStateClass = AGS_PhantomTwins::StaticClass();
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
        if (TPTLocalPlayerSaveGame->IdentifyMapData.MapType == EMapType::None)
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
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle,this, &ThisClass::DelayedInitializeSaveTargets, 0.1f, false);
}

void AGM_PhantomTwins::DelayedInitializeSaveTargets()
{
    UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
	SaveGameManager->InitializeSaveTargets();
    SaveGameManager->ApplyActorSaveGame();
}

void AGM_PhantomTwins::PostLogin(APlayerController* NewPlayer)
{
    UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
    SaveGameManager->InitializeSavePlayer();

    Super::PostLogin(NewPlayer);
    TotalPlayerCount++;
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

AActor* AGM_PhantomTwins::ChoosePlayerStart_Implementation(AController* Player)
{

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AGM_PhantomTwins::NotifyPlayerClickedGameStop(FName LevelName,FName PrintingName)
{
    DestinationLevelName = LevelName;
    ShowGameStopUI();
}

void AGM_PhantomTwins::ShowGameStopUI()
{
    SetAllPlayerUIMode(true);
    UGameplayStatics::SetGamePaused(GetWorld(), true);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("ESC"), false, EMessageTargetType::Multicast);
    ServerPC->SetWidget(TEXT("GameStop"), true, EMessageTargetType::Multicast);
}

void AGM_PhantomTwins::NotifyPlayerAgreeWithGameStop(int32 HostSelect, int32 ClientSelect)
{
    if (TotalPlayerCount == 1)
    {
        if (HostSelect == 1)
        {
            ShowLoadingScene();
            SeverToLevel(DestinationLevelName, false);
        }
        else if (HostSelect == 2)
        {
            ShowResumeCountUI();
        }
    }
    if (HostSelect == 1 && ClientSelect == 1) 
    {
        UGameplayStatics::SetGamePaused(GetWorld(), false);
		// 그냥  타이머를 넣으니까 멀티캐스트가 안됨. 안되는게 아니라 타이머가 끝나기 전에 트래블이 동시에 일어나기 때문에 안되는 거였음....
    	FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
            {
                ShowLoadingScene();
                SeverToLevel(DestinationLevelName, false);
            },
            1.f,
            false
        );
    }
    else if ((HostSelect != 0 && ClientSelect != 0) && (HostSelect == 2 || ClientSelect == 2))
    {
        ShowResumeCountUI();
    }
}

void AGM_PhantomTwins::ShowResumeCountUI()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("GameStop"), false, EMessageTargetType::Multicast);
    ServerPC->SetWidget(TEXT("ResumeCount"), true, EMessageTargetType::Multicast);
}

void AGM_PhantomTwins::ResumePlay()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APC_Player* ServerPC = Cast< APC_Player>(PC);

    ServerPC->SetWidget(TEXT("ResumeCount"), false, EMessageTargetType::Multicast);
    SetAllPlayerUIMode(false);
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

void AGM_PhantomTwins::ShowGameOverUI()
{
    SetAllPlayerUIMode(true);
    // 게임 시간을 정지하는 방법... 게임시간 여기에서 정지시키니까 트래블도 안됨.
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
    		APC_Player* ServerPC = Cast< APC_Player>(PC);
            ServerPC->SetWidget(TEXT("GameOver"), true, EMessageTargetType::Multicast);
        },
        1.f,
        false
    );
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
        RestartWithDelay(1.f);
    }
}

void AGM_PhantomTwins::RestartWithDelay(float Delay)
{
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            ShowLoadingScene();
            FString PackageName = GetWorld()->GetOutermost()->GetName();
            FString CleanPath = FPackageName::GetLongPackagePath(PackageName);
            FString MapBaseName = FPackageName::GetShortName(PackageName);


            FString TravelURL = CleanPath + TEXT("/") + MapBaseName + TEXT("?listen");
            GetWorld()->ServerTravel(TravelURL, false);

        }, Delay, false);
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
        APC_Player* PlayerPC = Cast<APC_Player>(PC);
        NULLCHECK_RETURN_LOG(PlayerPC, OutGameLog, Error, );

        PlayerPC->Client_SetUIInputMode(bIsUIMode);
    }
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


void AGM_PhantomTwins::SeverToLevel(const FName LevelName, bool bAbsolute, bool bIsListen)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC->HasAuthority()) return;

    //FString LevelPathWithListen = bIsListen ? LevelName.ToString() + TEXT("?listen") : LevelName.ToString();
    FString LevelPathWithListen = bIsListen ? (TEXT("Game/Maps/%s?listen"), *LevelName.ToString()) : LevelName.ToString();

    TPT_LOG(GameRuleLog, Log, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
    GetWorld()->ServerTravel(LevelPathWithListen, bAbsolute);
}
