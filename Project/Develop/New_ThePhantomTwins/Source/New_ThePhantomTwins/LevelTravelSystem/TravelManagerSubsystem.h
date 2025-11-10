// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TravelManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTravelManagerSubsystem, Log, All);

class ALevelTravelNetProxy;

UCLASS()
class NEW_THEPHANTOMTWINS_API UTravelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** 트래블 요청 (어디서든 호출 가능) */
    UFUNCTION(BlueprintCallable)
    void TravelToLevel(const FString& TargetMap, TSubclassOf<UUserWidget> WidgetClass, bool bServerTravel = true);

    /** 로딩맵에서 호출됨 */
    UFUNCTION(BlueprintCallable)
    void OnLoadingLevelReady();

    UFUNCTION(BlueprintCallable)
    bool GetCachedTargetMap(TSoftObjectPtr<UWorld>& TargetWorld);

    UFUNCTION(BlueprintCallable)
    FString GetTargetMap();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LoadingMapPath = "Invalid";

    UFUNCTION(BlueprintCallable)
    int32 GetUserCount();
    void PostLogin(APlayerController* NewPlayer);
    void Logout(AController* NewPlayer);

    //UFUNCTION()
    //void EnsureProxyOnServer(UWorld* World);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ALevelTravelNetProxy> ProxyClass;
           
    void CheckAllPlayerReady(APlayerController* NewPlayer);
    void FinishTravel();
private:
    UPROPERTY()
    TMap<TWeakObjectPtr<APlayerController>, TWeakObjectPtr<ALevelTravelNetProxy>> PerPCProxy;

    void TravelToLoadingMap();
    void HandlePostLoadMap(UWorld* World);
    bool ShouldHandleWorld(const UWorld* World);
    ALevelTravelNetProxy* SpawnOwnedProxyFor(APlayerController* PC, bool bAttachToPC);

private:
    int32 EnsureUserCount = 0;
    int32 UserCount = 0;
    FString CachedTargetMap;
    TSubclassOf<UUserWidget> CachedWidgetClass;
    bool bCachedServerTravel = true;
    bool bPostLoadEndPlayTravelLevel = false;
    bool bIsServerTravel = false;
    bool bIsTravel = false;
};
