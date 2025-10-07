// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "MonsterInfomationData.generated.h"


USTRUCT(BlueprintType)
struct FMonsterInfomationData : public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TObjectPtr<UTexture2D> MonsterIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText MonsterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TArray<FText> Description;
};
