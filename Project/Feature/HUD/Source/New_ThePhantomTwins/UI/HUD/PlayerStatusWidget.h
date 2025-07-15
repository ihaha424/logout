// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    void SetHP(float HP);
    void SetMental(float Mental);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> HPBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> MentalBar;

    //UPROPERTY(meta = (BindWidget))
    //TObjectPtr<class UImage> PlayerPortrait_Image;
};
