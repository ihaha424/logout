// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerWidgetInterface.generated.h"

class UMyPlayerUserWidget;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THE_PHANTOM_TWINS_API IPlayerWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetupCharacterWidget(UMyPlayerUserWidget* UserWidget) = 0;
};
