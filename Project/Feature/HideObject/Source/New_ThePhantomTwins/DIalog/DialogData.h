// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "DialogData.generated.h"


USTRUCT(BlueprintType)
struct FDialogData : public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText Text;
};
