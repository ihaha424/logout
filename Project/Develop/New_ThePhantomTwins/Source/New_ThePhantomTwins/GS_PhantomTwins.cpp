// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_PhantomTwins.h"

#include "GM_PhantomTwins.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"

#include "Kismet/KismetSystemLibrary.h"

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
    DOREPLIFETIME(AGS_PhantomTwins, HostSelect);
    DOREPLIFETIME(AGS_PhantomTwins, ClientSelect);
}

void AGS_PhantomTwins::AddCollectedItem(AActor* DataFragment, int32 Delta)
{
    if (!HasAuthority()) return;

    CoreCount = FMath::Max(0, CoreCount + Delta);

    CollectedItemCountChanged.Broadcast(CoreCount);
    DataFragmentChanged.Broadcast(DataFragment);
}

void AGS_PhantomTwins::MarkBossSpawned(AActor* InBoss)
{
    if (!HasAuthority() || bBossSpawned) return;

    bBossSpawned = true;
    BossActor = InBoss;

    GameTime = GetServerWorldTimeSeconds();
}

void AGS_PhantomTwins::SetCharacterClickedRestart(bool bIsClicked, bool bIsHost)
{
	TPT_LOG(OutGameLog, Log, TEXT("bIsClicked : %d, bIsHost : %d"), bIsClicked, bIsHost);
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
}

void AGS_PhantomTwins::OnRep_SetCharacterClickedRestart()
{
    TPT_LOG(OutGameLog, Log, TEXT("bIsHostClickedRestart : %d, bIsClientClickedRestart : %d"), bIsHostClickedRestart, bIsClientClickedRestart);
    OnClickedRestartChanged.Broadcast(bIsHostClickedRestart, bIsClientClickedRestart);
}

void AGS_PhantomTwins::SetCharacterClickedGameStop(FName LevelName)
{
    DestinationLevelName = LevelName;
    OnClickedGameStopChanged.Broadcast(DestinationLevelName);
}

void AGS_PhantomTwins::SetCharacterAgreeWithGameStop(int32 Select, bool bIsHost)
{
    UKismetSystemLibrary::PrintString(this, TEXT("tlqkf"));
	TPT_LOG(OutGameLog, Log, TEXT("Select : %d, bIsHost : %d"), Select, bIsHost);
    if (!HasAuthority()) return;

    if (bIsHost)
    {
        FString temp = FString::Printf(TEXT("HostSelect : %d, Select : %d"), HostSelect, Select);
        UKismetSystemLibrary::PrintString(this, temp);
        HostSelect = Select;
        FString temp2 = FString::Printf(TEXT("HostSelect : %d, Select : %d"), HostSelect, Select);
        UKismetSystemLibrary::PrintString(this, temp2);
    }
    else
    {
        FString temp = FString::Printf(TEXT("ClientSelect : %d, Select : %d"), ClientSelect, Select);
        UKismetSystemLibrary::PrintString(this, temp);
        ClientSelect = Select;
        FString temp2 = FString::Printf(TEXT("ClientSelect : %d, Select : %d"), ClientSelect, Select);
        UKismetSystemLibrary::PrintString(this, temp2);
    }
    OnClickedAgreeWithGameStopChanged.Broadcast(HostSelect, ClientSelect);
}

void AGS_PhantomTwins::OnRep_SetCharacterClickedGameStop()
{
    OnClickedGameStopChanged.Broadcast(DestinationLevelName);
}

void AGS_PhantomTwins::OnRep_SetCharacterAgreeWithGameStop()
{
    TPT_LOG(OutGameLog, Log, TEXT("HostSelect : %d, ClientSelect : %d"), HostSelect, ClientSelect);
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
}
