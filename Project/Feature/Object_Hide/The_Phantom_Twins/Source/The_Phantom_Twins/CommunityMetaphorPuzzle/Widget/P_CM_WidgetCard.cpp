// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_WidgetCard.h"
#include "../P_CM_Log.h"
#include "../P_CM_PuzzleActor.h"
#include "Components/TextBlock.h"
#include "../DataAsset/P_CM_CardData.h"

void UP_CM_WidgetCard::Effect_Implementation(bool IsToggle)
{
    if (IsToggle)
    {
        this->SetColorAndOpacity({ 255,0,0,1 });
    }
    else
    {
        this->SetColorAndOpacity({ 255,255,255,1 });
    }
}

FReply UP_CM_WidgetCard::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        OnClicked();
    }
    return FReply::Unhandled();
}

void UP_CM_WidgetCard::OnClicked()
{
    if (PuzzleOwner)
    {
        PuzzleOwner->HandleClick(CardIndex);
    }
}

bool UP_CM_WidgetCard::InitialData(const UP_CM_CardData* Data, int32 CardIndexInput)
{
    if (!Data)
    {
        UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Initialize fail: UP_CM_WidgetCard: Data is nullptr."));
        return false;
    }

    CardIndex = CardIndexInput;
    TextValueData = Data->Text;
    LogicalValueData = FText::AsNumber(Data->LogicValue);
    EmotionValueData = FText::AsNumber(Data->EmotionValue);
    
    return true;
}

void UP_CM_WidgetCard::NativeConstruct()
{
    Super::NativeConstruct();

    bIsFocusable = true;

    TextValue->SetText(TextValueData);
    LogicalValue->SetText(LogicalValueData);
    EmotionValue->SetText(EmotionValueData);
}

void UP_CM_WidgetCard::SetPuzzle(AP_CM_PuzzleActor* PuzzleActor)
{
    PuzzleOwner = PuzzleActor;
}
