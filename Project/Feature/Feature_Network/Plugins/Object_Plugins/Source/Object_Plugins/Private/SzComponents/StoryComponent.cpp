// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/StoryComponent.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Blueprint/UserWidget.h"

UStoryComponent::UStoryComponent()
{
    UE_LOG(LogTemp, Log, TEXT("UStoryComponent::UStoryComponent()"));

	PrimaryComponentTick.bCanEverTick = false;
}

void UStoryComponent::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("UStoryComponent::BeginPlay()"));

	Super::BeginPlay();

}

void UStoryComponent::Execute(APawn* Interactor)
{
    UE_LOG(LogTemp, Log, TEXT("UStoryComponent::Execute"));

    if (StoryActionType == EStoryActionType::PlaySequence && SequenceToPlay)
    {
        FMovieSceneSequencePlaybackSettings PlaybackSettings;
        ALevelSequenceActor* OutActor = nullptr;

        ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
            GetWorld(),
            SequenceToPlay,
            PlaybackSettings,
            OutActor
        );

        if (SequencePlayer)
        {
            SequencePlayer->Play();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unable to create level sequence player"));
        }
    }
    else if (StoryActionType == EStoryActionType::ShowUI && WidgetToShow)
    {
        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetToShow);
        if (Widget)
        {
            Widget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Unable to create widget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid action selected or asset not assigned!"));
    }
}
