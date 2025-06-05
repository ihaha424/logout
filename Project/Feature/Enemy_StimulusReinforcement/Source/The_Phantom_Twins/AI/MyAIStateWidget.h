// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIInterface.h"
#include "Blueprint/UserWidget.h"
#include "MyAIStateWidget.generated.h"

class UImage;

UCLASS()
class THE_PHANTOM_TWINS_API UMyAIStateWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void SetState(EAIStateWidget AIState);
protected:
    UPROPERTY(meta = (BindWidget))
    UImage* QuestionMark;

    UPROPERTY(meta = (BindWidget))
    UImage* ExclamationMark;
};
