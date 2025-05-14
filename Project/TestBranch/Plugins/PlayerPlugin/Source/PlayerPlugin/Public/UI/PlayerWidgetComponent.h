// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERPLUGIN_API UPlayerWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:

	virtual void InitWidget() override;
};
