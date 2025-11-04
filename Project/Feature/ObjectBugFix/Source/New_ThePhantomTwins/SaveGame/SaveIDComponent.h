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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid SaveId;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& E) override
    {
		Super::PostEditChangeProperty(E);
        if (!SaveId.IsValid())
        {
            SaveId = FGuid::NewGuid();
        }
    }

    virtual void OnComponentCreated() override
    {
        Super::OnComponentCreated();

        if (!SaveId.IsValid())
        {
            SaveId = FGuid::NewGuid();
        }
    }

#endif

	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		if (!SaveId.IsValid())
		{
			SaveId = FGuid::NewGuid();
		}
	}
};
