// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "Sound/SoundBase.h"
#include "TriggerDialogData.generated.h"

USTRUCT(BlueprintType)
struct FTriggerDialogData : public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TObjectPtr<USoundBase> SoundBase;
};
