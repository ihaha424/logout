// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTravelSystem/TravelManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "LevelTravelSystem/TravelBPLibrary.h"
#include "LevelTravelSystem/TravelWidgetInterface.h"

DEFINE_LOG_CATEGORY(LogTravelManagerSubsystem);

void UTravelManagerSubsystem::TravelToLevel(const FString& TargetMap, TSubclassOf<UUserWidget> WidgetClass, bool bServerTravel)
{
    if (LoadingMapPath == "Invalid")
        UE_LOG(LogTravelManagerSubsystem, Error, TEXT("TravelManagerSubsystem::LoadingMapPath is Invalid."));

    UWorld* World = GetWorld();
    if (!World) 
        return;

    CachedTargetMap = TargetMap;
    CachedWidgetClass = WidgetClass;
    bCachedServerTravel = bServerTravel;
    bPostLoadEndPlayTravelLevel = false;

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
        if (!bServerTravel)
            UTravelBPLibrary::ClientTravel(UGameplayStatics::GetPlayerController(World, 0), LoadingMap, TRAVEL_Absolute);
        else
            UE_LOG(LogTravelManagerSubsystem, Log, TEXT("Client: waiting for server to travel..."));
        break;

    default:
        // Standalone or PIE
        if (bServerTravel)
            UGameplayStatics::OpenLevel(World, FName(*LoadingMap), true, TEXT("listen"));
        else
            UGameplayStatics::OpenLevel(World, FName(*LoadingMap));
        break;
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
        UGameplayStatics::OpenLevel(World, FName(*CachedTargetMap));
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

        return true;
    }
    return false;
}

int32 UTravelManagerSubsystem::GetUserCount()
{
    return UserCount;
}

void UTravelManagerSubsystem::PostLogin()
{
    UserCount++;
}

void UTravelManagerSubsystem::Logout()
{
    UserCount--;
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
