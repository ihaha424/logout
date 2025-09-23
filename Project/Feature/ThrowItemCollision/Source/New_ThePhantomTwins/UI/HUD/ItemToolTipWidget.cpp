// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemToolTipWidget.h"
#include "Components/TextBlock.h"

void UItemToolTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemToolTipWidget::SetItemName(EItemType eItemType)
{
    if (!ItemDataTable) return;

    ItemName->SetText(FText::GetEmpty());

    // 데이터테이블에서 모든 row 조회
    const TArray<FName> RowNames = ItemDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FItemDataTable* Row = ItemDataTable->FindRow<FItemDataTable>(RowName, TEXT("SetItemName"));
        if (Row && Row->ItemType == eItemType)
        {
            // 이름 값을 FText로 변환해 widget에 표시
            ItemName->SetText(FText::FromName(RowName));
            return; // 해당 아이템 타입을 찾았으므로 루프 종료
        }
    }

    // 해당 타입을 못 찾았을 경우 기본값 혹은 빈 문자열 처리
    ItemName->SetText(FText::GetEmpty());
}

void UItemToolTipWidget::SetItemDescription(EItemType eItemType)
{
    if (!ItemDataTable) return;

    ItemDescription->SetText(FText::GetEmpty());

    const TArray<FName> RowNames = ItemDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FItemDataTable* Row = ItemDataTable->FindRow<FItemDataTable>(RowName, TEXT("SetItemDescription"));
        if (Row && Row->ItemType == eItemType)
        {
            // 설명 값이 있다면 FText로 변환 후 표시
            ItemDescription->SetText(Row->Description);
            return;
        }
    }

    // 해당 타입을 못 찾았을 경우 기본값 혹은 빈 문자열 처리
    ItemDescription->SetText(FText::GetEmpty());
}