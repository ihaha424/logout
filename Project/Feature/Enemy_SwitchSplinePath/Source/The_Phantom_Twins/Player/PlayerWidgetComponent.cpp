// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidgetComponent.h"
#include "MyPlayerUserWidget.h"

void UPlayerWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UMyPlayerUserWidget* UserWidget = Cast<UMyPlayerUserWidget>(GetWidget());
	if (UserWidget)
	{
		UserWidget->SetOwningActor(GetOwner());
	}
}
