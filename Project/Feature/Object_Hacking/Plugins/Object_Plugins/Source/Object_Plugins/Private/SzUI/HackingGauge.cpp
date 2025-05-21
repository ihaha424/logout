// Fill out your copyright notice in the Description page of Project Settings.


#include "SzUI/HackingGauge.h"
#include "Components/TextBlock.h"

void UHackingGauge::NativeConstruct()
{
    Super::NativeConstruct();

    if (TimeText)
    {
        TimeText->SetText(FText::FromString(TEXT("Holding Time: 0.00s")));
    }
}

void UHackingGauge::UpdateHoldTime(float Time)
{
    if (!TimeText) return;

    FString Formatted = FString::Printf(TEXT("Holding Time: %.2f s"), Time);
    TimeText->SetText(FText::FromString(Formatted));
}


