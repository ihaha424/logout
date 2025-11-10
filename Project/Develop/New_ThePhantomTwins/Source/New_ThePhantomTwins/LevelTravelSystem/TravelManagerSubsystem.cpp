// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTravelSystem/TravelManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "LevelTravelSystem/TravelBPLibrary.h"
#include "LevelTravelSystem/TravelWidgetInterface.h"
#include "LevelTravelSystem/LevelTravelNetProxy.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY(LogTravelManagerSubsystem);

void UTravelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ProxyClass = ALevelTravelNetProxy::StaticClass();
}

void UTravelManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UTravelManagerSubsystem::TravelToLevel(const FString& TargetMap, TSubclassOf<UUserWidget> WidgetClass, bool bServerTravel)
{
    if (LoadingMapPath == "Invalid")
        UE_LOG(LogTravelManagerSubsystem, Error, TEXT("TravelManagerSubsystem::LoadingMapPath is Invalid."));
    if (bIsTravel)
        UE_LOG(LogTravelManagerSubsystem, Log, TEXT("TravelManagerSubsystem::LoadingMapPath is already Travel."));

    UWorld* World = GetWorld();
    if (!World) 
        return;

    CachedTargetMap = TargetMap;
    CachedWidgetClass = WidgetClass;
    bCachedServerTravel = bServerTravel;
    bPostLoadEndPlayTravelLevel = false;
    bIsServerTravel = bServerTravel;
    EnsureUserCount = 0;
    bIsTravel = true;

    // ---- 1) Fade In ----
    if (WidgetClass && World->GetNetMode() != NM_DedicatedServer)
    {
        if (UUserWidget* Widget = CreateWidget<UUserWidget>(World, WidgetClass))
        {
            Widget->AddToViewport();
            if (Widget->GetClass()->ImplementsInterface(UTravelWidgetInterface::StaticClass()))
                ITravelWidgetInterface::Execute_BeginPlayTravelLevel(Widget);
        }
    }

    // TODO: ALevelTravelNetProxy의 C2S_RequestTravelToLevel 호출
    // TODO: ALevelTravelNetProxy에서 클라이언트들과 서버의 모든 인원이 준비완료를 하면 UTravelManagerSubsystem의 EnsureProxyOnServer호출
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    {
        ALevelTravelNetProxy* OwnedProxy = nullptr;
        for (TActorIterator<ALevelTravelNetProxy> It(World); It; ++It)
        {
            if (IsValid(*It) && It->GetOwner() == PC) 
            { 
                OwnedProxy = *It; 
                break; 
            }
        }
        if (OwnedProxy)
        {
            OwnedProxy->C2S_RequestTravelToLevel(PC);
        }
        else
        {
            UE_LOG(LogTravelManagerSubsystem, Warning, TEXT("No owned proxy for local PC yet."));
        }
    }


}

void UTravelManagerSubsystem::OnLoadingLevelReady()
{
    UWorld* World = GetWorld();
    if (!World || CachedTargetMap.IsEmpty()) 
        return;

    UE_LOG(LogTravelManagerSubsystem, Log, TEXT("OnLoadingLevelReady: Mode=%d, Map=%s"), (int32)World->GetNetMode(), *CachedTargetMap);

    // ---- 3) 실제 이동 ----
    switch (World->GetNetMode())
    {
    case NM_ListenServer:
        World->ServerTravel(CachedTargetMap + TEXT("?listen"));
        break;

    case NM_DedicatedServer:
        World->ServerTravel(CachedTargetMap);
        break;

    case NM_Client:
        if (!bCachedServerTravel)
        {
            UTravelBPLibrary::ClientTravel(UGameplayStatics::GetPlayerController(World, 0), CachedTargetMap, TRAVEL_Absolute);
        }
        else
        {
            UE_LOG(LogTravelManagerSubsystem, Log, TEXT("Client: waiting for server's ServerTravel..."));
        }
        break;

    default:
        UTravelBPLibrary::ClientTravel(UGameplayStatics::GetPlayerController(World, 0), CachedTargetMap, TRAVEL_Absolute);
        break;
    }

    // ---- 4) 새 맵 진입 후 FadeOut ----
    // EndPlayTravelLevel(FadeOut)은 직접 원하는 타이밍에 호출 해주는 것.
    //if (World->GetNetMode() != NM_DedicatedServer)
    //{
    //    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTravelManagerSubsystem::HandlePostLoadMap);
    //}
}

bool UTravelManagerSubsystem::GetCachedTargetMap(TSoftObjectPtr<UWorld>& TargetWorld)
{
    if (!CachedTargetMap.IsEmpty())
    {
        FSoftObjectPath Path(CachedTargetMap);
        TargetWorld = TSoftObjectPtr<UWorld>(Path);
        if(TargetWorld.IsValid())
            return true;
    }
    return false;
}

FString UTravelManagerSubsystem::GetTargetMap()
{
    return CachedTargetMap;
}

int32 UTravelManagerSubsystem::GetUserCount()
{
    return UserCount;
}

void UTravelManagerSubsystem::PostLogin(APlayerController* NewPlayer)
{
    if (!NewPlayer) return;

    UserCount++;
    // 중복 방지
    if (TWeakObjectPtr<ALevelTravelNetProxy>* Found = PerPCProxy.Find(NewPlayer))
    {
        if (Found->IsValid()) return;
    }

    SpawnOwnedProxyFor(NewPlayer, /*bAttachToPC=*/true);
}

void UTravelManagerSubsystem::Logout(AController* NewPlayer)
{
    UserCount--;
    if (APlayerController* PC = Cast<APlayerController>(NewPlayer))
    {
        if (TWeakObjectPtr<ALevelTravelNetProxy>* Found = PerPCProxy.Find(PC))
        {
            if (Found->IsValid())
            {
                Found->Get()->Destroy();
            }
        }
        PerPCProxy.Remove(PC);
    }
}

//void UTravelManagerSubsystem::EnsureProxyOnServer(UWorld* World)
//{
//    if (!ProxyClass) return;
//
//    // 서버에서만 스폰 (PIE 서버/리스닝/데디케이티드 모두 포함)
//    const ENetMode NM = World->GetNetMode();
//    const bool bIsServer = (NM == NM_DedicatedServer || NM == NM_ListenServer || NM == NM_Standalone);
//    if (!bIsServer) return;
//
//    // 월드당 1개만
//    for (TActorIterator<ALevelTravelNetProxy> It(World); It; ++It)
//    {
//        return; // 이미 존재
//    }
//
//    FActorSpawnParameters Params;
//    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//    Params.ObjectFlags |= RF_Transient;      // 맵 트래블 시 자동 파괴/재생성 원하면 유지
//#if WITH_EDITOR
//    // 에디터 월드에서 ‘저장’에 끼지 않게
//    if (World->WorldType == EWorldType::Editor)
//    {
//        Params.ObjectFlags |= RF_Transactional; // 필요 시
//    }
//#endif
//
//    Proxy = World->SpawnActor<ALevelTravelNetProxy>(ProxyClass, FTransform::Identity, Params);
//    if (Proxy)
//    {
//        Proxy->SetReplicates(true);
//        Proxy->SetReplicateMovement(false);
//        Proxy->SetActorHiddenInGame(true);
//    }
//}

void UTravelManagerSubsystem::TravelToLoadingMap()
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    // ---- 2) LoadingLevel 이동 ----
    const FString LoadingMap = LoadingMapPath;

    switch (World->GetNetMode())
    {
    case NM_ListenServer:
        UTravelBPLibrary::ServerTravel(World, LoadingMap, false, true);
        break;

    case NM_DedicatedServer:
        UTravelBPLibrary::ServerTravel(World, LoadingMap, false, false);
        break;

    case NM_Client:
        if (!bIsServerTravel)
            UTravelBPLibrary::ClientTravel(UGameplayStatics::GetPlayerController(World, 0), LoadingMap, TRAVEL_Absolute);
        else
            UE_LOG(LogTravelManagerSubsystem, Log, TEXT("Client: waiting for server to travel..."));
        break;

    default:
        // Standalone or PIE
        if (bIsServerTravel)
            UGameplayStatics::OpenLevel(World, FName(*LoadingMap), true, TEXT("listen"));
        else
            UGameplayStatics::OpenLevel(World, FName(*LoadingMap));
        break;
    }
}

void UTravelManagerSubsystem::CheckAllPlayerReady(APlayerController* NewPlayer)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    // 서버만 스폰
    const ENetMode NM = World->GetNetMode();
    const bool bServer = (NM == NM_ListenServer || NM == NM_DedicatedServer || NM == NM_Standalone);
    if (!bServer)
        return;

    if (auto* it = PerPCProxy.Find(NewPlayer))
    {
        EnsureUserCount++;
    }

    if (EnsureUserCount == UserCount)
        TravelToLoadingMap();
    // 서버면 준비 다 확인하고 TravelToLoadingMap();
}

void UTravelManagerSubsystem::FinishTravel()
{
    bIsTravel = false;
    EnsureUserCount = 0;
}

void UTravelManagerSubsystem::HandlePostLoadMap(UWorld* World)
{
    if (!IsValid(World)) 
        return;
    if (World->GetNetMode() == NM_DedicatedServer) 
        return;
    if (!CachedWidgetClass) 
        return;
    if (bPostLoadEndPlayTravelLevel) 
        return;

    bPostLoadEndPlayTravelLevel = true;

    // 1프레임 뒤에 실행 — UMG/World 초기화 완료 후 안전하게 위젯 생성
    FTimerHandle TempHandle;
    World->GetTimerManager().SetTimer(
        TempHandle,
        [this]()
        {
            if (!GetGameInstance() || !CachedWidgetClass) return;
            UUserWidget* Widget = CreateWidget<UUserWidget>(GetGameInstance(), CachedWidgetClass);
            if (!Widget) 
                return;

            Widget->AddToViewport();
            if (Widget->GetClass()->ImplementsInterface(UTravelWidgetInterface::StaticClass()))
                ITravelWidgetInterface::Execute_EndPlayTravelLevel(Widget);
        },
        0.01f, false
    );
}

bool UTravelManagerSubsystem::ShouldHandleWorld(const UWorld* World)
{
    if (!World) return false;

    switch (World->WorldType)
    {
    case EWorldType::Game:
    case EWorldType::PIE:             // Play-In-Editor
    case EWorldType::GamePreview:     // 별도 프리뷰 실행
    case EWorldType::EditorPreview:   // 에디터 미리보기
    case EWorldType::Editor:          // 에디터 월드(원하면 포함)
        break;
    default:
        return false; // None/Inactive/Unknown 등 제외
    }

    // 월드가 파괴 중이면 제외
    if (World->bIsTearingDown) return false;

    return true;
}

ALevelTravelNetProxy* UTravelManagerSubsystem::SpawnOwnedProxyFor(APlayerController* PC, bool bAttachToPC)
{
    if (!PC || !ProxyClass) 
        return nullptr;

    UWorld* World = PC->GetWorld();
    if (!World) 
        return nullptr;

    // 서버만 스폰
    const ENetMode NM = World->GetNetMode();
    const bool bServer = (NM == NM_ListenServer || NM == NM_DedicatedServer || NM == NM_Standalone);
    if (!bServer) 
        return nullptr;

    // 혹시 이미 있는지 한번 더 확인(안전)
    for (TActorIterator<ALevelTravelNetProxy> It(World); It; ++It)
    {
        if (IsValid(*It) && It->GetOwner() == PC)
        {
            PerPCProxy.FindOrAdd(PC) = *It;
            return *It;
        }
    }

    FActorSpawnParameters P;
    P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    P.Owner = PC;                        // ★ 오너십 부여
    P.ObjectFlags |= RF_Transient;       // 트래블 시 파괴되어도 OK (재보장 로직이 있으므로)

    ALevelTravelNetProxy* Proxy = World->SpawnActor<ALevelTravelNetProxy>(ProxyClass, FTransform::Identity, P);
    if (!Proxy) 
        return nullptr;

    // 네트 설정은 프록시 생성자에서 했지만, 안전하게 다시 명시
    Proxy->SetReplicates(true);
    Proxy->bOnlyRelevantToOwner = true;
    Proxy->bNetUseOwnerRelevancy = true;
    Proxy->SetReplicateMovement(false);
    Proxy->SetActorHiddenInGame(true);

    //// (선택) PC에 부착 — 월드에 남기기 싫다면 KeepWorld/KeepRelative 선택
    //if (bAttachToPC)
    //{
    //    Proxy->AttachToActor(PC, FAttachmentTransformRules::KeepWorldTransform);
    //}

    PerPCProxy.FindOrAdd(PC) = Proxy;
    return Proxy;
}

