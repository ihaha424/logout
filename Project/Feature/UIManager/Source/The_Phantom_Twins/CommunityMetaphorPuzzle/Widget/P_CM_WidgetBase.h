// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P_CM_WidgetBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class THE_PHANTOM_TWINS_API UP_CM_WidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /**
     * @brief :
            CallBack Effect Fucntion
     */
    UFUNCTION(BlueprintNativeEvent, Category = "Effect")
    void Effect(bool IsToggle);

    virtual void Effect_Implementation(bool IsToggle);
};
