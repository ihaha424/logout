// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSkillWidget.h"
#include "Components/Image.h"
#include "Components/StackBox.h"
#include "Blueprint/UserWidget.h"

void UPlayerSkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerSkillWidget::SetActiveSkillIcon(UTexture2D* ActiveSkillIcon)
{
	if (Img_ActiveSkillIcon && ActiveSkillIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(ActiveSkillIcon);
		Img_ActiveSkillIcon->SetBrush(Brush);
	}
}

void UPlayerSkillWidget::SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon)
{
	if (Img_PassiveSkillIcon && PassiveSkillIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(PassiveSkillIcon);
		Img_PassiveSkillIcon->SetBrush(Brush);
	}
}

void UPlayerSkillWidget::ShowCoreEnergy(int32 ShowCnt)
{
	if (!CoreEnergyStackBox) return;

	int32 ChildrenNum = CoreEnergyStackBox->GetChildrenCount();
	for (int32 i = 0; i < ChildrenNum; ++i)
	{
		UWidget* Child = CoreEnergyStackBox->GetChildAt(i);

		if (Child)
		{
			// ¸Ç À§(ÀÎµ¦½º 0)ºÎÅÍ Â÷·Ê·Î ¼û°ÜÁü
			Child->SetVisibility(i >= (ChildrenNum - ShowCnt) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}
