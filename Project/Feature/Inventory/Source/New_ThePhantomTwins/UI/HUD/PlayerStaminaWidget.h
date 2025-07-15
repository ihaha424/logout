// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStaminaWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UPlayerStaminaWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    void SetStamina(int32 Stamina);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> StaminaBar;
};
