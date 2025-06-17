#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "StorySaveState.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API UStorySaveState : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TMap<FName, FString> SavedData;
};
