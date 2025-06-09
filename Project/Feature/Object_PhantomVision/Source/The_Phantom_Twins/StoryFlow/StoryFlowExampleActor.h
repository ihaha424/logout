#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryFlowExampleActor.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API AStoryFlowExampleActor : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

private:
    FGuid FlowSubscriptionId;

    void OnFlowChanged(FName Key, const UObject* Value);
};
