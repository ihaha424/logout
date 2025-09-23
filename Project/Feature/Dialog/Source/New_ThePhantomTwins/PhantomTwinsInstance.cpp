// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"
#include "Tags/TPTGameplayTags.h"
#include "DialogManager/DialogManager.h"

void UPhantomTwinsInstance::Init()
{
	Super::Init();
	FTPTGameplayTags::Get();
	DialogManager = NewObject<UDialogManager>(this);

	DialogManager->Initialize(Initialize);
}
