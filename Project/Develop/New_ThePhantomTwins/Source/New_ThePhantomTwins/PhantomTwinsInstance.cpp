// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"
#include "Tags/TPTGameplayTags.h"
#include "GameFramework/GameUserSettings.h"
#include "Log/TPTLog.h"
#include "DialogManager/DialogManager.h"

void UPhantomTwinsInstance::Init()
{
	Super::Init();
	FTPTGameplayTags::Get();

	// 전체화면 모드 적용
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		FIntPoint DesktopRes = Settings->GetDesktopResolution();
		Settings->SetScreenResolution(DesktopRes);
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);  // 또는 WindowedFullscreen / Windowed
		Settings->ApplySettings(false); // 즉시 적용 (true로 하면 저장 포함)
	}

	DialogManager = NewObject<UDialogManager>(this);

	DialogManager->Initialize(Initialize);
}
