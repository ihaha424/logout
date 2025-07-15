// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatusWidget.h"
#include "Components/ProgressBar.h"

void UPlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerStatusWidget::SetHP(float HP)
{
	if (!HPBar) return;

	HPBar->SetPercent(HP);

}

void UPlayerStatusWidget::SetMental(float Mental)
{
	if (!MentalBar) return;

	MentalBar->SetPercent(Mental);
}
