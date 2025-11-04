// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DroneStatWidget.h"

#include <string>

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/DataFragment.h"
#include "GS_PhantomTwins.h"
#include "Log/TPTLog.h"

void UDroneStatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>())
    {
        ItemChangedHandle = GS->OnCollectedItemCountChanged().AddUObject(
            this, &UDroneStatWidget::SetClearItemCount);
		SetClearItemCount(GS->DataFragmentCount);
    }
    else
    {
        SetClearItemCount(0);
    }
    SetMaxClearItem();
}

void UDroneStatWidget::NativeDestruct()
{
    if (AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>())
    {
        if (ItemChangedHandle.IsValid())
            GS->OnCollectedItemCountChanged().Remove(ItemChangedHandle);
    }

    Super::NativeDestruct();
}

void UDroneStatWidget::SetMaxClearItem()
{
    // 레벨에 배치되어 있는 ADataFragment 들을 찾아서 총 갯수를 체크
    TArray<AActor*> FoundActors;
    if (GetWorld() && ClearItemClass)
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClearItemClass, FoundActors);
        MaxClearItem = FoundActors.Num();
    }
    else
    {
        MaxClearItem = 5;
    }
}

void UDroneStatWidget::SetHP(int32 HP)
{
    if (TEXT_HP)
    {
        TEXT_HP->SetText(FText::AsNumber(HP));
    }
}

void UDroneStatWidget::SetMP(int32 MP)
{
    if (TEXT_MP)
    {
        TEXT_MP->SetText(FText::AsNumber(MP));
    }
}

void UDroneStatWidget::SetClearItemCount(int32 CurrentClearItem)
{
    if (TEXT_ClearItem)
    {
        FString CountStr = FString::Printf(TEXT("%d/%d"), CurrentClearItem, MaxClearItem);
        TEXT_ClearItem->SetText(FText::FromString(CountStr));
    }
}
