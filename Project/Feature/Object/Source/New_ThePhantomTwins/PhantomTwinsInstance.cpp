// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"

#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Kismet/GameplayStatics.h"
#include "DialogManager/DialogManager.h"
#include "SaveGame/TPTSaveGameManager.h"
#include "SettingManager/SettingSystem.h"

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

	DialogManager = NewObject<UDialogManager>(this);

	DialogManager->Initialize(Initialize);
	GetSubsystem<UTPTSaveGameManager>();
}
