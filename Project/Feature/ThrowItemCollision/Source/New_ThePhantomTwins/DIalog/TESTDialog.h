// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogManager/DialogManager.h"
#include "TESTDialog.generated.h"


UCLASS()
class NEW_THEPHANTOMTWINS_API UTESTDialog : public UObject
{
	GENERATED_BODY()

public:
	TObjectPtr<UUserWidget> TestWidget;

};
