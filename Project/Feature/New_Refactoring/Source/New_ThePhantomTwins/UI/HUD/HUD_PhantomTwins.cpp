// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_PhantomTwins.h"
#include "PlayerStatusWidget.h"
#include "Blueprint/UserWidget.h"

void AHUD_PhantomTwins::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->AddToViewport();
        }
    }
}

void AHUD_PhantomTwins::UpdateHP(float HP)
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->SetHP(HP); // UHPBarWidget이 실제 UI에 반영
    }
}
