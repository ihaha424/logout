// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/StoryComponent.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

UStoryComponent::UStoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UStoryComponent::Execute(APawn* Interactor)
{
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

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                FTimerDelegate::CreateLambda([Widget]()
                    {
                        if (Widget && Widget->IsInViewport())
                        {
                            Widget->RemoveFromParent();
                        }
                    }),
                WidgetDuration,
                false
            );
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
