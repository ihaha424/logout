// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FUIMessageRow.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FUIMessageTRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UIText")
    FText Message;
};