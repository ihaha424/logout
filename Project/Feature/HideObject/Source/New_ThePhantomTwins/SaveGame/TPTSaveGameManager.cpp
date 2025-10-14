// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTSaveGameManager.h"

#include "TPTLocalPlayerSaveGame.h"
#include "TPTSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/Door.h"

//void UTPTSaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
//{
//    Super::Initialize(Collection);
//
//    // 플레이어별 저장 객체 2개 초기화
//    PlayerSaveGames.SetNum(2);
//    for (int32 i = 0; i < 2; i++)
//    {
//        PlayerSaveGames[i] = NewObject<UTPTLocalPlayerSaveGame>(this);
//    }
//
//    GameSaveGame = NewObject<UTPTSaveGame>(this);
//}
//
//void UTPTSaveGameManager::SaveAll()
//{
//    if (!GetWorld() || !GameSaveGame) return;
//    // 문 액터 상태 저장 (예: ADoorActor)
//    TArray<AActor*> DoorActors;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), DoorActors);
//    for (AActor* Actor : DoorActors)
//    {
//        ADoor* Door = Cast<ADoor>(Actor);
//        if (Door)
//        {
//            int32 DoorID = Door->GetUniqueID();  // 또는 커스텀 식별자
//            bool bIsOpened = Door->bIsActived();       // Door 상태 획득 함수 예시
//            bool 
//                FGuid::NewGuid()
//            GameSaveGame->DoorStates.Add(DoorID, bIsOpened);
//        }
//    }
//
//    // 아이템 박스 상태 저장 (예: AItemBoxActor)
//    TArray<AActor*> ItemBoxActors;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemBoxActor::StaticClass(), ItemBoxActors);
//    for (AActor* Actor : ItemBoxActors)
//    {
//        AItemBoxActor* ItemBox = Cast<AItemBoxActor>(Actor);
//        if (ItemBox)
//        {
//            int32 ItemBoxID = ItemBox->GetUniqueID();
//            bool bIsOpened = ItemBox->IsOpened();  // 상태 획득 함수
//            GameSaveGame->ItemBoxStates.Add(ItemBoxID, bIsOpened);
//        }
//    }
//
//    // 획득한 아이템 상태 (예: 수집된 아이템 리스트나 Actor)
//    TArray<AActor*> ItemActors;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemActor::StaticClass(), ItemActors);
//    for (AActor* Actor : ItemActors)
//    {
//        AItemActor* Item = Cast<AItemActor>(Actor);
//        if (Item)
//        {
//            int32 ItemID = Item->GetUniqueID();
//            bool bIsPickedUp = Item->IsPickedUp();  // 예시 상태 함수
//            GameSaveGame->ItemPickupStates.Add(ItemID, bIsPickedUp);
//        }
//    }
//
//    // 부서지는 오브젝트 상태 (예: ADestructibleActor)
//    TArray<AActor*> DestructibleActors;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADestructibleActor::StaticClass(), DestructibleActors);
//    for (AActor* Actor : DestructibleActors)
//    {
//        ADestructibleActor* Destructible = Cast<ADestructibleActor>(Actor);
//        if (Destructible)
//        {
//            int32 ObjID = Destructible->GetUniqueID();
//            bool bIsHidden = Destructible->IsHidden();  // 상태 추출 함수 예시
//            GameSaveGame->DestructibleObjectStates.Add(ObjID, bIsHidden);
//        }
//    }
//
//    // 데이터 조각 갯수 증가 (예시)
//    GameSaveGame->DataFragmentNum++;
//}
//
//void UTPTSaveGameManager::LoadAll()
//{
//    for (int32 i = 0; i < PlayerSaveGames.Num(); i++)
//    {
//        FString SlotName = FString::Printf(TEXT("PlayerSaveSlot%d"), i);
//        if (UGameplayStatics::DoesSaveGameExist(SlotName, i))
//        {
//            auto LoadedSave = Cast<UTPTLocalPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, i));
//            if (LoadedSave)
//            {
//                PlayerSaveGames[i] = LoadedSave;
//            }
//        }
//    }
//
//    if (UGameplayStatics::DoesSaveGameExist(TEXT("GlobalSaveSlot"), 0))
//    {
//        auto LoadedSave = Cast<UTPTSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GlobalSaveSlot"), 0));
//        if (LoadedSave)
//        {
//            GameSaveGame = LoadedSave;
//        }
//    }
//}
