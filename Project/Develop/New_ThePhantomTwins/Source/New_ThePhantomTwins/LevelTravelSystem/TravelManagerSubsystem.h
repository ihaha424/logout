// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TravelManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTravelManagerSubsystem, Log, All);

UCLASS()
class NEW_THEPHANTOMTWINS_API UTravelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
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
    void PostLogin();
    void Logout();
           
private:
    UFUNCTION()
    void HandlePostLoadMap(UWorld* World);


private:
    int32 UserCount = 0;
    int32 TravelUserCount = 0;
    FString CachedTargetMap;
    TSubclassOf<UUserWidget> CachedWidgetClass;
    bool bCachedServerTravel = true;
    bool bPostLoadEndPlayTravelLevel = false;
};
