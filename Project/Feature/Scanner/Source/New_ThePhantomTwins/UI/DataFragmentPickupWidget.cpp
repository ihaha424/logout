// Fill out your copyright notice in the Description page of Project Settings.


#include "DataFragmentPickupWidget.h"
#include "Components/TextBlock.h"

void UDataFragmentPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDataFragmentPickupWidget::SetText(const FText& Text)
{
    if (TextWidget)
    {
        TextWidget->SetText(Text);
    }
}
