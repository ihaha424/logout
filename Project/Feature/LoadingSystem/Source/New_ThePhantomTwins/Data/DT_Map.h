// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Log/TPTLog.h"
#include "DT_Map.generated.h"


USTRUCT(BlueprintType)
struct FMapDataTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UWorld> MapPackagePath;
};


UCLASS()
class NEW_THEPHANTOMTWINS_API UDT_Map : public UDataTable
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, Category = "MapDataTable")
    static FName ConvertRowToMapName( const FMapDataTableRow& RowData)
    {
        FString FullPath = RowData.MapPackagePath.ToSoftObjectPath().ToString();

        FString LeftPart;
        if (FullPath.Split(TEXT("."), &LeftPart, nullptr))
        {
            return FName(LeftPart);
        }
        return FName(FullPath);
    }
};
