// Fill out your copyright notice in the Description page of Project Settings.


#include "SzUI/PhantomVisionWidget.h"
#include "Components/TextBlock.h"

void UPhantomVisionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CCTVIDTxt)
    {
        CCTVIDTxt->SetText(FText::FromString(TEXT("Hacked CCTV : ")));
    }
}

void UPhantomVisionWidget::SetCCTVIDTxt(int32 CurrentCCTVID)
{
    if (!CCTVIDTxt) return;

    FString Formatted = FString::Printf(TEXT("Hacked CCTV : %d"), CurrentCCTVID);
    CCTVIDTxt->SetText(FText::FromString(Formatted));
}
