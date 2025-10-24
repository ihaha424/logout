// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_PhantomTwins.h"

#include "GM_PhantomTwins.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"

#include "Kismet/KismetSystemLibrary.h"
#include "SaveGame/TPTSaveGame.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"

#include "Decal/StickerManager.h"

AStickerManager* AGS_PhantomTwins::GetStickerManager()
{
    if(!StickerManager.IsValid())
        StickerManager = GetWorld()->SpawnActor<AStickerManager>();
    return StickerManager.Get();
}

void AGS_PhantomTwins::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_PhantomTwins, GameTime);
    DOREPLIFETIME(AGS_PhantomTwins, CoreCount);
    DOREPLIFETIME(AGS_PhantomTwins, bBossSpawned);
    DOREPLIFETIME(AGS_PhantomTwins, BossActor);
    DOREPLIFETIME(AGS_PhantomTwins, bIsHostClickedRestart);
    DOREPLIFETIME(AGS_PhantomTwins, bIsClientClickedRestart);
    DOREPLIFETIME(AGS_PhantomTwins, DestinationLevelName);
    DOREPLIFETIME(AGS_PhantomTwins, PrintingMapName);
    DOREPLIFETIME(AGS_PhantomTwins, HostSelect);
    DOREPLIFETIME(AGS_PhantomTwins, ClientSelect);
}

void AGS_PhantomTwins::AddCollectedItem(AActor* DataFragment, int32 Delta)
{
    if (!HasAuthority()) return;

    CoreCount = FMath::Max(0, CoreCount + Delta);

    CollectedItemCountChanged.Broadcast(CoreCount);
    DynamicCollectedItemCountChanged.Broadcast(CoreCount);
    DataFragmentChanged.Broadcast(DataFragment);
}

void AGS_PhantomTwins::MarkBossSpawned(AActor* InBoss)
{
    if (!HasAuthority() || bBossSpawned) return;

    bBossSpawned = true;
    BossActor = InBoss;

    GameTime = GetServerWorldTimeSeconds();
}

void AGS_PhantomTwins::C2S_SetCharacterClickedRestart_Implementation(bool bIsClicked, bool bIsHost)
{
    if (!HasAuthority()) return;

    if (bIsHost)
    {
	    bIsHostClickedRestart = bIsClicked;
    }
    else
    {
	    bIsClientClickedRestart = bIsClicked;
    }
    OnClickedRestartChanged.Broadcast(bIsHostClickedRestart, bIsClientClickedRestart);
	S2A_SetCharacterClickedRestart(bIsClicked, bIsHost);
}

void AGS_PhantomTwins::S2A_SetCharacterClickedRestart_Implementation(bool bIsClicked, bool bIsHost)
{
    if (bIsHost)
    {
        bIsHostClickedRestart = bIsClicked;
    }
    else
    {
        bIsClientClickedRestart = bIsClicked;
    }
    OnClickedRestartChanged.Broadcast(bIsHostClickedRestart, bIsClientClickedRestart);
}

void AGS_PhantomTwins::SetCharacterClickedGameStop(FName LevelName, FName PrintingName)
{
    DestinationLevelName = LevelName;
    PrintingMapName = PrintingName;
    OnClickedGameStopChanged.Broadcast(DestinationLevelName, PrintingMapName);
}

void AGS_PhantomTwins::OnRep_SetCharacterClickedGameStop()
{
    OnClickedGameStopChanged.Broadcast(DestinationLevelName, PrintingMapName);
}

void AGS_PhantomTwins::C2S_SetCharacterAgreeWithGameStop_Implementation(int32 Select, bool bIsHost)
{
    if (!HasAuthority()) return;

    if (bIsHost)
    {
        HostSelect = Select;
    }
    else
    {
        ClientSelect = Select;
    }
    OnClickedAgreeWithGameStopChanged.Broadcast(HostSelect, ClientSelect);
	S2A_SetCharacterAgreeWithGameStop(Select, bIsHost);
}

void AGS_PhantomTwins::S2A_SetCharacterAgreeWithGameStop_Implementation(int32 Select, bool bIsHost)
{
	if (bIsHost)
    {
        HostSelect = Select;
    }
    else
    {
        ClientSelect = Select;
    }
	OnClickedAgreeWithGameStopChanged.Broadcast(HostSelect, ClientSelect);
}

void AGS_PhantomTwins::OnRep_BossSpawned()
{
    if (bBossSpawned)
    {
        OnBossSpawned.Broadcast(BossActor);
    }
}

void AGS_PhantomTwins::OnRep_CollectedItemCount()
{
    CollectedItemCountChanged.Broadcast(CoreCount);
    DynamicCollectedItemCountChanged.Broadcast(CoreCount);
}
