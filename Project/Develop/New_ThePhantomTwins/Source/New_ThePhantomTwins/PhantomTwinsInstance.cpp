// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"

#include "Tags/TPTGameplayTags.h"
#include "DialogManager/DialogManager.h"
#include "SaveGame/TPTSaveGameManager.h"
#include "SettingManager/SettingSystem.h"
#include "LevelTravelSystem/TravelManagerSubsystem.h"

void UPhantomTwinsInstance::Init()
{
	Super::Init();
	FTPTGameplayTags::Get();

	USettingSystem* SubSystem =	GetSubsystem<USettingSystem>();
	UTravelManagerSubsystem* TravelManagerSubsystem = GetSubsystem<UTravelManagerSubsystem>();

	SubSystem->SetSoundSoundMix(BaseSoundMix);
	for (auto It = SoundClassList.CreateIterator(); It; ++It)
	{
		SubSystem->AddSoundClassList(It.Value(), It.Key());
	}
	SubSystem->VolumeInitialize();

	DialogManager = NewObject<UDialogManager>(this);

	DialogManager->Initialize(Initialize);
	GetSubsystem<UTPTSaveGameManager>();

	/*
		FadeInOutInstance = CreateWidget<UFHFadeInOut>(this, FadeInOutWidgetClass);
		FadeInOutInstance->SetVisibility(ESlateVisibility::Collapsed);

		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFHGameInstance::ShowFadeWidget);
	*/
}
