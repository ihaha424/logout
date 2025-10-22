// Fill out your copyright notice in the Description page of Project Settings.


#include "Decal/StickerManager.h"

AStickerManager::AStickerManager()
{
    // bReplicates = true; // 필요 시, 통계 노출 용
    bAlwaysRelevant = true;      // 간단히 전체에 보이게
    SetReplicatingMovement(false);
}

void AStickerManager::RegisterSticker(int32 OwnerPlayerId, AStickerActor* NewActor)
{
    if (!HasAuthority() || OwnerPlayerId == -1) return;

    EnforcePlayerLimit(OwnerPlayerId);
    AddSticker(OwnerPlayerId, NewActor);
}

void AStickerManager::EnforcePlayerLimit(int32 OwnerPlayerId)
{
    TArray<FStickerRecord>& List = PerPlayerStickers.FindOrAdd(OwnerPlayerId);

    // 비어있는 슬롯 정리(파괴된 액터 제거)
    List.RemoveAll([](const FStickerRecord& S) 
        { 
            return !S.Actor.IsValid(); 
        });

    while (MaxPerPlayer > 0 && List.Num() >= MaxPerPlayer)
    {
        RemoveOldestOf(OwnerPlayerId);
    }
}

void AStickerManager::AddSticker(int32 OwnerPlayerId, AStickerActor* NewActor)
{
    TArray<FStickerRecord>& List = PerPlayerStickers.FindOrAdd(OwnerPlayerId);
    FStickerRecord Rec;
    Rec.Actor = NewActor;
    Rec.SpawnTime = GetWorld()->GetTimeSeconds();
    List.Add(Rec);
}

void AStickerManager::RemoveOldestOf(int32 OwnerPlayerId)
{
    if (TArray<FStickerRecord>* Ptr = PerPlayerStickers.Find(OwnerPlayerId))
    {
        // FIFO: 가장 먼저 찍은 것(SpawnTime이 가장 작은 것) 제거
        Ptr->RemoveAll([](const FStickerRecord& S) 
            { 
                return !S.Actor.IsValid(); 
            });

        if (Ptr->Num() == 0) 
            return;

        int32 OldestIdx = 0;
        double OldestTime = (*Ptr)[0].SpawnTime;
        for (int32 i = 1; i < Ptr->Num(); ++i)
        {
            if ((*Ptr)[i].SpawnTime < OldestTime)
            {
                OldestTime = (*Ptr)[i].SpawnTime;
                OldestIdx = i;
            }
        }

        if (AStickerActor* Old = (*Ptr)[OldestIdx].Actor.Get())
            Old->Destroy();
        Ptr->RemoveAt(OldestIdx);
    }
}
