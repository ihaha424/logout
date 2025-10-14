// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utility/BossSpawner.h"
#include "GM_PhantomTwins.h"
#include "GS_PhantomTwins.h"

ABossSpawner::ABossSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABossSpawner::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority()) return;

    AGM_PhantomTwins* GM = GetWorld()->GetAuthGameMode<AGM_PhantomTwins>();
    GM->PreferredSpawnActor = this;
}

void ABossSpawner::SpawnBossOnce()
{
    if (!HasAuthority()) return;

    AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
    if (!GS || GS->bBossSpawned) return;
    if (!BossClass) return;

    const FTransform Xf = GetActorTransform();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* Boss = GetWorld()->SpawnActor<AActor>(BossClass, Xf, Params);
    if (!Boss) return;

    GS->MarkBossSpawned(Boss);
    SpawnBoss();
}



