// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_PhantomTwins.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AGS_PhantomTwins::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_PhantomTwins, GameTime);
    DOREPLIFETIME(AGS_PhantomTwins, CoreCount);
    DOREPLIFETIME(AGS_PhantomTwins, bBossSpawned);
    DOREPLIFETIME(AGS_PhantomTwins, BossActor);
    DOREPLIFETIME(AGS_PhantomTwins, bIsHostClickedRestart);
    DOREPLIFETIME(AGS_PhantomTwins, bIsClientClickedRestart);
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

void AGS_PhantomTwins::SetCharacterClickedRestart(bool bIsHostClicked, bool bIsClientClicked)
{
    if (!HasAuthority()) return;

	bIsHostClickedRestart = bIsHostClicked;
	bIsClientClickedRestart = bIsClientClicked;

    OnRep_SetCharacterClickedRestart();
}

void AGS_PhantomTwins::OnRep_SetCharacterClickedRestart()
{
    OnClickedRestartChanged.Broadcast(bIsHostClickedRestart, bIsClientClickedRestart);
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
