// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoadingPrepSubsystem.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogLoadingPrepSubsystem, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingPrepComplete);

UCLASS()
class NEW_THEPHANTOMTWINS_API ULoadingPrepSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    // 에디터에서 지정: 다음에 열 맵과, 미리 올려둘 에셋들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preload")
    TSoftObjectPtr<UWorld> TargetWorld;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preload")
    TArray<TSoftObjectPtr<UObject>> PreloadAssets;

    UPROPERTY(BlueprintAssignable, Category = "Preload")
    FOnLoadingPrepComplete OnLoadingPrepComplete;

    UFUNCTION(BlueprintCallable) 
    void BeginPreload();
    UFUNCTION(BlueprintCallable) 
    void CancelPreload();
    UFUNCTION(BlueprintCallable) 
    float GetProgress() const;
    UFUNCTION(BlueprintCallable) 
    bool IsAllLoaded() const { return bDone; }
    UFUNCTION(BlueprintCallable)
    void ReleaseKeepAlive();

    UPROPERTY() 
    TArray<UObject*> KeepAlive;

private:
    TArray<TSharedPtr<struct FStreamableHandle>> Handles;
    bool bDone = false;
    bool bStarted = false;

    void OnAnyHandleComplete();
};
