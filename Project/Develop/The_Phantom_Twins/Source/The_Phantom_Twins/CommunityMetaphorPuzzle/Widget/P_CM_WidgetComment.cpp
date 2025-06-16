// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_WidgetComment.h"
#include "../P_CM_Log.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../DataAsset/P_CM_CommentData.h"

bool UP_CM_WidgetComment::InitialData(const UP_CM_CommentData* Data)
{
    if (!Data)
    {
        UE_LOG(LogCommunityMetaphorPuzzle, Warning, TEXT("Initialize fail: UP_CM_WidgetComment: Data is nullptr."));
        return false;
    }

    TextData = Data->Text;
    IconData = Data->Icon;

    return true;
}

void UP_CM_WidgetComment::NativeConstruct()
{
    Super::NativeConstruct();

    Text->SetText(TextData);
    Icon->SetBrushFromTexture(IconData);
}

void UP_CM_WidgetComment::Effect_Implementation(bool IsToggle)
{
}
