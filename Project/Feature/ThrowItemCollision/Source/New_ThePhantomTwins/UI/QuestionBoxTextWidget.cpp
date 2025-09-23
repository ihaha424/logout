// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestionBoxTextWidget.h"
#include "Components/TextBlock.h"

void UQuestionBoxTextWidget::SetText(const FText& Text)
{
    if (TextWidget)
    {
        TextWidget->SetText(Text);
    }
}
