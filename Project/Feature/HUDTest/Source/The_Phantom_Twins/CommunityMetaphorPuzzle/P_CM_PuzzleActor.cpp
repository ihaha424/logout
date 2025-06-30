// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_PuzzleActor.h"
#include "P_CM_Log.h"
#include "Widget/P_CM_WidgetBoard.h"
#include "Net/UnrealNetwork.h"
#include "../Player/PlayerDefaultController.h"

AP_CM_PuzzleActor::AP_CM_PuzzleActor()
{
    bReplicates = true;
}

void AP_CM_PuzzleActor::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(TimerHandle_Limit, this, &AP_CM_PuzzleActor::OnTimeExpired, 60.f, false);
    OwningWidget->SetOwner(this);
}

void AP_CM_PuzzleActor::InitializePuzzle(const UP_CM_PuzzleData* Data)
{
    Logic.Initialize(Data);
    OwningWidget->InitializeData(Data);
}

void AP_CM_PuzzleActor::HandleClick(int32 CardIndex)
{
    if (!HasAuthority())
    {
        UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("HandleClick Client"));
        if (nullptr == GetOwner())
        {
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                APlayerDefaultController* MyPC = Cast<APlayerDefaultController>(PC);
                MyPC->C2S_SetOwnerActor(PC, this);
                UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("OwnerSetting Client"));
            }
        }
    }
    C2S_HandleClick(CardIndex);
}

void AP_CM_PuzzleActor::C2S_HandleClick_Implementation(int32 CardIndex)
{
    UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("C2S_HandleClick_Implementation"));
    Logic.HandleClick(CardIndex);
    ReplicatedState = GetCurrentPuzzleState();
    if (HasAuthority())
    {
        OnRep_PuzzleState();
    }
    ForceNetUpdate(); // Immediately trigger replication updates
    if (Logic.CheckPuzzle())
    {
        UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Puzzle Success."));
        bIsSuccessPuzzle = true;
    }
    else
    {
        bIsSuccessPuzzle = false;
    }
}

void AP_CM_PuzzleActor::TravelToLevel(const FName LevelName)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!IsValid(PC)) return;

    if (PC->HasAuthority())
    {
        OpenAndTravelToLevel(LevelName);
    }
    else
    {
        // Features are provided, but calls are recommended only on hosts.
        C2S_ClearButton(LevelName);
    }
}

void AP_CM_PuzzleActor::C2S_ClearButton_Implementation(const FName LevelName)
{
    OpenAndTravelToLevel(LevelName);
}

void AP_CM_PuzzleActor::OpenAndTravelToLevel(const FName LevelName)
{
    if (!bIsSuccessPuzzle)
        return;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC->HasAuthority()) return;

    FString LevelPathWithListen = LevelName.ToString() + TEXT("?listen");

    UE_LOG(LogTemp, Log, TEXT("LevelPathWithListen: %s"), *LevelPathWithListen);
    GetWorld()->ServerTravel(LevelPathWithListen, false);
}

FPuzzleState AP_CM_PuzzleActor::GetCurrentPuzzleState() const
{
    FPuzzleState State;
    for (const auto& Card : Logic.GetCards())
    {
        State.bCardFlipped.Add(Card.IsToggle);
    }
    State.LogicalValue = Logic.GetLogicValue();
    State.EmotionValue = Logic.GetEmotionValue();
    return State;
}

void AP_CM_PuzzleActor::RestorePuzzleState(const FPuzzleState& State)
{
    // TODO: 복구 로직 작성
}

void AP_CM_PuzzleActor::OnRep_PuzzleState()
{
    if (OwningWidget && OwningWidget->GetIsVisible())
    {
        OwningWidget->UpdateUIFromState(ReplicatedState);
    }
}

void AP_CM_PuzzleActor::OnTimeExpired()
{
    if(!Logic.CheckPuzzle())
        UE_LOG(LogCommunityMetaphorPuzzle, Log, TEXT("Time's up!"));
}

void AP_CM_PuzzleActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AP_CM_PuzzleActor, ReplicatedState);
}
