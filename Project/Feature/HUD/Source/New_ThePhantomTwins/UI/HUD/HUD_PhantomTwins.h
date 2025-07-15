// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_PhantomTwins.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AHUD_PhantomTwins : public AHUD
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerStatus")
    void UpdateHP(float HP);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPlayerStatusWidget> PlayerStatusWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPlayerStatusWidget> PlayerStatusWidget;
};
