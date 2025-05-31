// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAIInterface : public UInterface
{
	GENERATED_BODY()
};
DECLARE_DELEGATE(FAIAttackFinished);
/**
 * 
 */
class THE_PHANTOM_TWINS_API IAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	virtual void SetAIAttackDelegate(const FAIAttackFinished& InOnAttackFinished) = 0;
	virtual void AttackByAI() = 0;

	FAIAttackFinished OnAttackFinished;
};

UENUM(BlueprintType)
enum class EMyAIState : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Suspicion	UMETA(DisplayName = "Suspicion"),
	Combat		UMETA(DisplayName = "Combat"),
	Hacked		UMETA(DisplayName = "Hacked")
};