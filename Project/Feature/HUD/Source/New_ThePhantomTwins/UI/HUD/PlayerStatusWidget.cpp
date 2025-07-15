
#include "PlayerStatusWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UPlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerStatusWidget::SetHP(int32 HP)
{
	if (!HPBar) return;

	float HPRatio = FMath::Clamp(static_cast<float>(HP) / 100.0f, 0.0f, 1.0f);
	HPBar->SetPercent(HPRatio);

}

void UPlayerStatusWidget::SetMental(int32 Mental)
{
	if (!MentalBar) return;

	float MentalRatio = FMath::Clamp(static_cast<float>(Mental) / 100.0f, 0.0f, 1.0f);
	MentalBar->SetPercent(MentalRatio);
}

void UPlayerStatusWidget::SetCharPortrait(UTexture2D* PortraitTexture)
{
	if (CharPortrait && PortraitTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(PortraitTexture);
		CharPortrait->SetBrush(Brush);
	}
}
