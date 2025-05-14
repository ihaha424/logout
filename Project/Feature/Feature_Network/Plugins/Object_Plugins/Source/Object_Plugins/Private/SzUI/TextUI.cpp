// Fill out your copyright notice in the Description page of Project Settings.


#include "SzUI/TextUI.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"

UTextUI::UTextUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , RowName("B")
{

}

void UTextUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 데이터 테이블과 텍스트 블록이 있는지 확인
    if (UITextDataTable && TextTextN)
    {
        static const FString ContextString(TEXT("WB_TestUI"));
        FUIMessageTRow* Row = UITextDataTable->FindRow<FUIMessageTRow>(RowName, ContextString, true);

        if (Row)
        {
            TextTextN->SetText(Row->Message);
            UE_LOG(LogTemp, Display, TEXT("Found Row! Message: %s"), *Row->Message.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Row %s not found in DataTable"), *RowName.ToString());
        }
    }
    else
    {
        if (!UITextDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("UITextDataTable is not set! 에디터에서 데이터 테이블을 할당하세요."));
        }
        if (!TextTextN)
        {
            UE_LOG(LogTemp, Error, TEXT("TextTextN is not bound. UMG 디자이너에서 TextBlock 이름을 'TextTextN'으로 설정하세요."));
        }
    }
}