// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "PlayerSkillWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UPlayerSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    void SetActiveSkillIcon(UTexture2D* ActiveSkillIcon);
    void SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon);
    void ShowCoreEnergy(int32 ShowCnt);

protected:
	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> Img_ActiveSkillIcon;

	UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> Img_PassiveSkillIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UStackBox> CoreEnergyStackBox;
};
