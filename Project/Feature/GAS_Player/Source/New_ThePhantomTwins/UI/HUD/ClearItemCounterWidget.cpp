#include "ClearItemCounterWidget.h"
#include "Components/EditableText.h"

void UClearItemCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();

    SetClearItemCount(0);
}

void UClearItemCounterWidget::SetClearItemCount(int32 CurrentClearItem)
{
    if (TEXT_ClearItem)
    {
        FString CountStr = FString::Printf(TEXT("%d/%d"), CurrentClearItem, MaxClearItem);
        TEXT_ClearItem->SetText(FText::FromString(CountStr));
    }
}
