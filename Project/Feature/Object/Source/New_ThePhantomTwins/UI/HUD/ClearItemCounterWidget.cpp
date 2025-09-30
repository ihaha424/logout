#include "ClearItemCounterWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/DataFragment.h"

void UClearItemCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();
    SetMaxClearItem();
    SetClearItemCount(0);
}

void UClearItemCounterWidget::SetMaxClearItem()
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

void UClearItemCounterWidget::SetClearItemCount(int32 CurrentClearItem)
{
    if (TEXT_ClearItem)
    {
        FString CountStr = FString::Printf(TEXT("%d/%d"), CurrentClearItem, MaxClearItem);
        TEXT_ClearItem->SetText(FText::FromString(CountStr));
    }
}
