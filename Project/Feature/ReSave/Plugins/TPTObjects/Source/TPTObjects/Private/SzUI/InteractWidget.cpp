
#include "SzUI/InteractWidget.h"
#include "Components/TextBlock.h"

void UInteractWidget::SetText(const FText& Text)
{
    if (ActionText)
    {
        ActionText->SetText(Text);
    }
}
