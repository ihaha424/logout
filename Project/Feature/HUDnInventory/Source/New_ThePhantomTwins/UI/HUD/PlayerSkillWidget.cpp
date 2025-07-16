// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSkillWidget.h"
#include "Components/Image.h"
#include "Components/StackBox.h"
#include "Blueprint/UserWidget.h"

void UPlayerSkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//if (SkillPointsStackBox && SkillPointWidgetClass)
	//{
	//	UUserWidget* NewWidget = CreateWidget<UUserWidget>(this, SkillPointWidgetClass);
	//	SkillPointsStackBox->AddChildToStackBox(NewWidget);
	//}
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

void UPlayerSkillWidget::ShowSkillPoints(int32 ShowCnt)
{
	if (!SkillPointsStackBox) return;

	int32 ChildrenNum = SkillPointsStackBox->GetChildrenCount();
	for (int32 i = 0; i < ChildrenNum; ++i)
	{
		UWidget* Child = SkillPointsStackBox->GetChildAt(i);

		if (Child)
		{
			// ¸Ç À§(ÀÎµ¦½º 0)ºÎÅÍ Â÷·Ê·Î ¼û°ÜÁü
			Child->SetVisibility(i >= (ChildrenNum - ShowCnt) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}
