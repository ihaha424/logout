// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P_CM_PuzzleState.h"
#include "Logic/P_CM_Puzzle.h"
#include "P_CM_PuzzleActor.generated.h"

class UP_CM_WidgetBoard;

UCLASS()
class THE_PHANTOM_TWINS_API AP_CM_PuzzleActor : public AActor
{
    GENERATED_BODY()

public:
    AP_CM_PuzzleActor();

    virtual void BeginPlay() override;
    void InitializePuzzle(const UP_CM_PuzzleData* Data);

    void HandleClick(int32 CardIndex);
    UFUNCTION(Server, Reliable)
    void C2S_HandleClick(int32 CardIndex);
    void C2S_HandleClick_Implementation(int32 CardIndex);


    UFUNCTION(BlueprintCallable)
    void TravelToLevel(const FName LevelName);
    UFUNCTION(Server, Reliable)
    void C2S_ClearButton(const FName LevelName);
    void C2S_ClearButton_Implementation(const FName LevelName);
    void OpenAndTravelToLevel(const FName LevelName);

    UPROPERTY(ReplicatedUsing = OnRep_PuzzleState)
    FPuzzleState ReplicatedState;

    UFUNCTION()
    void OnRep_PuzzleState();

    FPuzzleState GetCurrentPuzzleState() const;
    void RestorePuzzleState(const FPuzzleState& State);

    UPROPERTY(EditAnywhere, Category = "Community Metaphor Puzzle")
    TObjectPtr<UP_CM_PuzzleData> InitializeData;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Community Metaphor Puzzle")
    TObjectPtr<UP_CM_WidgetBoard> OwningWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Community Metaphor Puzzle")
    FTimerHandle TimerHandle_Limit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Community Metaphor Puzzle")
    bool bIsSuccessPuzzle;

private:
    FP_CM_Puzzle Logic;

    void OnTimeExpired();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
