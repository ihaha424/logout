// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIStateWidget.h"
#include "Components/Image.h"



void UMyAIStateWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetState(EAIStateWidget::NoneMark);
}

void UMyAIStateWidget::SetState(EAIStateWidget AIState)
{
    if (!QuestionMark || !ExclamationMark) return;

    QuestionMark->SetVisibility(ESlateVisibility::Hidden);
    ExclamationMark->SetVisibility(ESlateVisibility::Hidden);

    switch (AIState)
    {
    case EAIStateWidget::QuestionMark:
        QuestionMark->SetVisibility(ESlateVisibility::Visible);
        ExclamationMark->SetVisibility(ESlateVisibility::Hidden);
        break;
    case EAIStateWidget::ExclamationMark:
        QuestionMark->SetVisibility(ESlateVisibility::Hidden);
        ExclamationMark->SetVisibility(ESlateVisibility::Visible);
        break;
	case EAIStateWidget::NoneMark:
        QuestionMark->SetVisibility(ESlateVisibility::Hidden);
        ExclamationMark->SetVisibility(ESlateVisibility::Hidden);
		break;
    default:
        QuestionMark->SetVisibility(ESlateVisibility::Hidden);
        ExclamationMark->SetVisibility(ESlateVisibility::Hidden);
        break;
    }


}
