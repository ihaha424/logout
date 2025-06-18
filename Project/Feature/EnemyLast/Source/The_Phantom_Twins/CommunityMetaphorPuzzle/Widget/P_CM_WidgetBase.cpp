// Fill out your copyright notice in the Description page of Project Settings.


#include "P_CM_WidgetBase.h"

void UP_CM_WidgetBase::Effect_Implementation(bool IsToggle)
{
	UE_LOG(LogTemp, Error, TEXT("Effect_Implementation() must be overridden in child!"));
	checkNoEntry();
}
