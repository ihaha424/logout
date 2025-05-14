// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "UIDataTableHelper.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API UUIDataTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    // 데이터 테이블에서 메시지 가져오기
    UFUNCTION(BlueprintCallable, Category = "UI|DataTable")
    static FText GetMessageFromDataTable(UDataTable* DataTable, FName RowName);
    
    // 데이터 테이블 메시지로 텍스트 블록 설정하기
    UFUNCTION(BlueprintCallable, Category = "UI|DataTable")
    static bool SetTextBlockFromDataTable(UDataTable* DataTable, FName RowName, UTextBlock* TextBlock);
};
