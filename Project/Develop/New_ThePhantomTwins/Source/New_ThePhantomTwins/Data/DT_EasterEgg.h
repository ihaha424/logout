// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_EasterEgg.generated.h"

USTRUCT(BlueprintType)
struct FEasterEggDataRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    FEasterEggDataRow()
    {}

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Part;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Comment;
};

UCLASS()
class NEW_THEPHANTOMTWINS_API UDT_EasterEgg : public UDataTable
{
	GENERATED_BODY()
	
};
