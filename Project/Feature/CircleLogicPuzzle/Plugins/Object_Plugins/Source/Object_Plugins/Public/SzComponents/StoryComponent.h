// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzComponents/InteractableComponent.h"
#include "StoryComponent.generated.h"

UENUM(BlueprintType)
enum class EStoryActionType : uint8
{
    PlaySequence	UMETA(DisplayName = "Play Sequence"),
    ShowUI			UMETA(DisplayName = "Show UI")
};

/**
 * 
 */
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UStoryComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
    UStoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void ExecuteSever(APawn* Interactor) override;
    virtual void ExecuteClient(APawn* Interactor) override;

    UFUNCTION(NetMulticast, Reliable)
    void S2A_LevelSequencePlay();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
    EStoryActionType StoryActionType = EStoryActionType::PlaySequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|LS")
    TObjectPtr<class ULevelSequence> SequenceToPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|UI")
    TSubclassOf<class UUserWidget> WidgetToShow;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story|UI")
    float WidgetDuration = 2.0f;
	
};
