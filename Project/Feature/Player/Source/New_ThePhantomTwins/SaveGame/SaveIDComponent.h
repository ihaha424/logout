// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveIDComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API USaveIDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGuid SaveId;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& E) override
    {
        if (!SaveId.IsValid())
        {
            SaveId = FGuid::NewGuid();
        }
    }
#endif
};
