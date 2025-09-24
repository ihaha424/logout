// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "PopUpData.generated.h"

USTRUCT(BlueprintType)
struct FPopUpData : public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TObjectPtr<UTexture2D> Image;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText Text;
};
