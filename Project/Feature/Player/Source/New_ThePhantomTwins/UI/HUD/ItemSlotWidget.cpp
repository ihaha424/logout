// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../../Objects/ItemData.h"

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemSlotWidget::ResetItemSlot()
{
	// 이미지도 텍스트도 NULL 이여야 함(아무것도 안떠야 함)
	if (ItemIcon)
	{
		// 빈 브러시로 초기화해서 아이콘 숨기기
        FSlateBrush EmptyBrush;
        EmptyBrush.SetResourceObject(nullptr); // 리소스를 명시적으로 제거
        EmptyBrush.TintColor = FLinearColor::Transparent; // 완전 투명
        ItemIcon->SetBrush(EmptyBrush);
	}

	if (ItemQuantity)
	{
		// 빈 텍스트로 초기화
		ItemQuantity->SetText(FText::GetEmpty());
	}
}

void UItemSlotWidget::SetItemIcon(EItemType eItemType)
{
    if (!ItemIcon || !ItemDataTable)
        return;

    const TArray<FName> RowNames = ItemDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FItemDataTable* Row = ItemDataTable->FindRow<FItemDataTable>(RowName, TEXT("SetItemIcon"));
        if (Row && Row->ItemType == eItemType)
        {
            if (Row->ItemIcon)
            {
                FSlateBrush IconBrush;
                IconBrush.SetResourceObject(Row->ItemIcon);
                ItemIcon->SetBrush(IconBrush);
            }
            else
            {
                // 아이콘 없으면 비우기
                //ItemIcon->SetBrush(FSlateBrush());

                // 아이콘 없으면 빨간색으로 칠하기
                FSlateBrush RedBrush;
                RedBrush.TintColor = FSlateColor(FLinearColor::Red);
                ItemIcon->SetBrush(RedBrush);
            }
            return;
        }
    }

    // 매칭되는 아이템이 없으면 아이콘 비우기
    ItemIcon->SetBrush(FSlateBrush());
}

void UItemSlotWidget::SetItemQuantity(int32 ItemStack)
{
	if (ItemQuantity)
	{
		ItemQuantity->SetText(FText::AsNumber(ItemStack));
	}
}