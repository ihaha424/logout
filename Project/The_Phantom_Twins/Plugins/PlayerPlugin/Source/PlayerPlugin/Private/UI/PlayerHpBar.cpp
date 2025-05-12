// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHpBar.h"
#include "Interface/PlayerWidgetInterface.h"
#include "Components/ProgressBar.h"

UPlayerHpBar::UPlayerHpBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void UPlayerHpBar::UpdateHpBar(float CurrentHp)
{

}

void UPlayerHpBar::NativeConstruct()
{
	Super::NativeConstruct();

	IPlayerWidgetInterface* PlayerWidgetInterface = Cast<IPlayerWidgetInterface>(OwningActor);
	if (PlayerWidgetInterface)
	{
		PlayerWidgetInterface->SetupCharacterWidget(this);
	}
}
