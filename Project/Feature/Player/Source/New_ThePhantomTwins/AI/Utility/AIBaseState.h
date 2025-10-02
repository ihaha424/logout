// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIBaseState : uint8
{
    Default		UMETA(DisplayName = "Default"),
    Suspicion	UMETA(DisplayName = "Suspicion"),
    Combat		UMETA(DisplayName = "Combat"),
    Stun		UMETA(DisplayName = "Stun"),
    Die         UMETA(DisplayName = "Die"),
};
