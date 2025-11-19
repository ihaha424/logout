// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"

#include "Tags/TPTGameplayTags.h"
#include "DialogManager/DialogManager.h"
#include "SaveGame/TPTSaveGameManager.h"
#include "SettingManager/SettingSystem.h"
#include "LevelTravelSystem/TravelManagerSubsystem.h"
#include "Blueprint/UserWidget.h"

void UPhantomTwinsInstance::Init()
{
	Super::Init();
	FTPTGameplayTags::Get();

	USettingSystem* SubSystem =	GetSubsystem<USettingSystem>();
	SubSystem->SetSoundSoundMix(BaseSoundMix);
	for (auto It = SoundClassList.CreateIterator(); It; ++It)
	{
		SubSystem->AddSoundClassList(It.Value(), It.Key());
	}
	SubSystem->VolumeInitialize();

	UTravelManagerSubsystem* TravelManagerSubsystem = GetSubsystem<UTravelManagerSubsystem>();
	UUserWidget* TravelWidgetInterface = CreateWidget<UUserWidget>(this, TravelWidgetInterfaceClass);
	TravelManagerSubsystem->SetTravelWidgetInterface(TravelWidgetInterface);

	DialogManager = NewObject<UDialogManager>(this);

	DialogManager->Initialize(Initialize);
	GetSubsystem<UTPTSaveGameManager>();

}
