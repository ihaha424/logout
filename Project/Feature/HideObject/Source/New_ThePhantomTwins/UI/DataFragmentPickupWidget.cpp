// Fill out your copyright notice in the Description page of Project Settings.


#include "DataFragmentPickupWidget.h"
#include "Components/TextBlock.h"

void UDataFragmentPickupWidget::SetText(const FText& Text)
{
    if (TextWidget)
    {
        TextWidget->SetText(Text);
    }
}
