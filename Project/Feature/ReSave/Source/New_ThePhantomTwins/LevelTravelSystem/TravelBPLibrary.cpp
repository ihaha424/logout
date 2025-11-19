// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTravelSystem/TravelBPLibrary.h"
#include "LevelTravelSystem/TravelManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"

void UTravelBPLibrary::TravelWithWidget(const UObject* WorldContextObject,
    const FString& TargetMap,
    TSubclassOf<UUserWidget> WidgetClass,
    bool bServerTravel)
{
    if (!WorldContextObject) return;

    if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        if (auto* Sub = GI->GetSubsystem<UTravelManagerSubsystem>())
        {
            Sub->TravelToLevel(TargetMap, WidgetClass, bServerTravel);
        }
    }
}


static FString AppendOptions(const FString& Base, const FString& Opt)
{
    if (Opt.IsEmpty()) return Base;
    FString Out = Base;
    Out += Base.Contains(TEXT("?")) ? TEXT("&") : TEXT("?");
    Out += Opt;
    return Out;
}

bool UTravelBPLibrary::ServerTravel(const UObject* WorldContextObject,
    const FString& MapPath,
    bool bUseSeamless,
    bool bKeepListen,
    const FString& ExtraOptions,
    bool bAbsolute,
    bool bSkipGameNotify)
{
    if (!WorldContextObject) return false;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (!World) return false;

    // 서버 권한 체크
    if (World->GetNetMode() == NM_Client)
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerTravel called on a client. Ignored."));
        return false;
    }

    // Seamless 트래블 설정
    if (AGameModeBase* GM = World->GetAuthGameMode())
    {
        GM->bUseSeamlessTravel = bUseSeamless;
    }

    // URL 구성
    FString URL = MapPath;                      // ex) /Game/Maps/Game01
    if (bKeepListen) URL = AppendOptions(URL, TEXT("listen"));
    if (!ExtraOptions.IsEmpty()) URL = AppendOptions(URL, ExtraOptions);

    return World->ServerTravel(URL, bAbsolute, bSkipGameNotify);
}

void UTravelBPLibrary::ClientTravel(APlayerController* PC,
    const FString& URL,
    TEnumAsByte<ETravelType> TravelType,
    bool bSeamless)
{
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("ClientTravel: PlayerController is null."));
        return;
    }
    PC->ClientTravel(URL, (ETravelType)TravelType.GetValue(), bSeamless);
}
