// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingSystem/LoadingPrepSubsystem.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

DEFINE_LOG_CATEGORY(LogLoadingPrepSubsystem);

void ULoadingPrepSubsystem::BeginPreload()
{
    if (bStarted) 
        return;

    bStarted = true;

    CancelPreload();
    bDone = false;

    TArray<FSoftObjectPath> Paths;
    // 1) 타깃 월드
    if (TargetWorld.ToSoftObjectPath().IsValid())
    {
        Paths.AddUnique(TargetWorld.ToSoftObjectPath());
    }

    // 2) 에디터에서 넣어둔 에셋들
    for (const TSoftObjectPtr<UObject>& Obj : PreloadAssets)
    {
        if (Obj.ToSoftObjectPath().IsValid())
        {
            Paths.AddUnique(Obj.ToSoftObjectPath());
        }
    }

    if (Paths.Num() == 0) 
    { 
        bDone = true; 
        bStarted = false;
        return;
    }

    auto& SM = UAssetManager::GetStreamableManager();
    // 한 번에 로드(진행률 집계가 쉬움)
    TSharedPtr<FStreamableHandle> H = SM.RequestAsyncLoad(
        Paths,
        FStreamableDelegate::CreateUObject(this, &ULoadingPrepSubsystem::OnAnyHandleComplete),
        FStreamableManager::AsyncLoadHighPriority
    );

    if (H.IsValid())
    {
        Handles.Add(H);
    }
}

void ULoadingPrepSubsystem::OnAnyHandleComplete()
{
    // 완료한 핸들의 로드된 에셋을 모아 GC 방지
    for (int32 i = Handles.Num() - 1; i >= 0; --i)
    {
        if (Handles[i].IsValid() && Handles[i]->HasLoadCompleted())
        {
            TArray<UObject*> Loaded;
            Handles[i]->GetLoadedAssets(Loaded);
            KeepAlive.Append(Loaded);
            Handles.RemoveAt(i);
        }
    }
    // 타깃 맵도 강참조
    if (TargetWorld.IsValid() && !KeepAlive.Contains(TargetWorld.Get()))
    {
        KeepAlive.Add(TargetWorld.Get());
    }

    if (Handles.Num() == 0)
    {
        bDone = true;
        bStarted = false;
        OnLoadingPrepComplete.Broadcast();
    }
    else
    {
        UE_LOG(LogLoadingPrepSubsystem, Warning, TEXT("OnAnyHandleComplete called but Handles is not empty!"));
    }
}

void ULoadingPrepSubsystem::CancelPreload()
{
    for (auto& H : Handles)
    {
        if (H.IsValid())
        {
            H->CancelHandle();
        }
    }
    Handles.Empty();
    KeepAlive.Empty();
    bDone = true;
    bStarted = false;
}

float ULoadingPrepSubsystem::GetProgress() const
{
    if (Handles.Num() == 0)
    {
        return bDone ? 1.f : 0.f;
    }

    float sum = 0.f; 
    int32 cnt = 0;
    for (const auto& H : Handles)
    {
        if (H.IsValid())
        {
            sum += H->GetProgress(); 
            ++cnt; 
        }
    }
    return cnt > 0 ? FMath::Clamp(sum / float(cnt), 0.f, 1.f) : 0.f;
}

void ULoadingPrepSubsystem::ReleaseKeepAlive()
{
    KeepAlive.Empty();
    Handles.Empty();
    bDone = true;
    bStarted = false;
}
