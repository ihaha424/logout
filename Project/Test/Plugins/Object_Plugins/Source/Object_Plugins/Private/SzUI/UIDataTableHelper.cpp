// Fill out your copyright notice in the Description page of Project Settings.


#include "SzUI/UIDataTableHelper.h"
#include "Components/TextBlock.h"
#include "FUIMessageRow.h"


// DataTable에서 RowName에 해당하는 메시지(FText)를 반환하는 함수
FText UUIDataTableHelper::GetMessageFromDataTable(UDataTable* DataTable, FName RowName)
{
    // 데이터 테이블이 없으면 경고 로그 출력 후 빈 텍스트 반환
    if (!DataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetMessageFromDataTable: DataTable is null"));
        return FText::FromString("");
    }

    static const FString ContextString(TEXT("GetMessageFromDataTable"));
    // DataTable에서 RowName에 해당하는 Row를 찾음
    FUIMessageTRow* Row = DataTable->FindRow<FUIMessageTRow>(RowName, ContextString, true);

    if (Row)
    {
        // Row를 찾았으면 메시지 반환
        return Row->Message;
    }

    // Row를 못 찾았으면 경고 로그 출력 후 빈 텍스트 반환
    UE_LOG(LogTemp, Warning, TEXT("Row %s not found in DataTable"), *RowName.ToString());
    return FText::FromString("");
}

// DataTable에서 RowName에 해당하는 메시지를 TextBlock에 세팅하는 함수
bool UUIDataTableHelper::SetTextBlockFromDataTable(UDataTable* DataTable, FName RowName, UTextBlock* TextBlock)
{
    // 데이터 테이블이나 텍스트 블록이 없으면 경고 로그 출력 후 false 반환
    if (!DataTable || !TextBlock)
    {
        if (!DataTable)
        {
            UE_LOG(LogTemp, Warning, TEXT("SetTextBlockFromDataTable: DataTable is null"));
        }
        if (!TextBlock)
        {
            UE_LOG(LogTemp, Warning, TEXT("SetTextBlockFromDataTable: TextBlock is null"));
        }
        return false;
    }

    static const FString ContextString(TEXT("SetTextBlockFromDataTable"));
    // DataTable에서 RowName에 해당하는 Row를 찾음
    FUIMessageTRow* Row = DataTable->FindRow<FUIMessageTRow>(RowName, ContextString, true);

    if (Row)
    {
        // Row를 찾았으면 메시지를 TextBlock에 세팅
        TextBlock->SetText(Row->Message);
        UE_LOG(LogTemp, Display, TEXT("Text set to: %s"), *Row->Message.ToString());
        return true;
    }

    // Row를 못 찾았으면 경고 로그 출력 후 false 반환
    UE_LOG(LogTemp, Warning, TEXT("Row %s not found in DataTable"), *RowName.ToString());
    return false;
}
