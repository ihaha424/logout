// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense_Hearing.h"
#include "TPTAISense_Hearing.generated.h"


enum Priority_Hearing
{

};

UCLASS()
class THE_PHANTOM_TWINS_API UTPTAISense_Hearing : public UAISense_Hearing
{
	GENERATED_BODY()

protected:
    virtual float Update() override;
};
