// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingManager/SettingSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "AudioDevice.h"

void USettingSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MouseInitialize();
	GraphicsInitialize();
	ResolutionInitialize();
}

void USettingSystem::Deinitialize()
{
	ResolutionInitialize();

	Super::Deinitialize();
}

void USettingSystem::AddSoundClassList(USoundClass* SoundClass, FName& ClassName)
{
	SoundClassList.Add(ClassName, SoundClass);
}

void USettingSystem::SetSoundSoundMix(USoundMix* SoundMix)
{
	BaseSoundMix = SoundMix;
}

void USettingSystem::VolumeInitialize()
{
	UGameplayStatics::SetBaseSoundMix(GetWorld(), BaseSoundMix);

	for (auto It = SoundClassList.CreateIterator(); It; ++It)
	{
		UGameplayStatics::SetSoundMixClassOverride(
			GetWorld(),
			BaseSoundMix,
			It.Value(),
			1.0f,
			1.0f,        
			1.0f,       
			true         
		);
	}

	UGameplayStatics::PushSoundMixModifier(GetWorld(), BaseSoundMix);
}

void USettingSystem::MouseInitialize()
{
	// TODO : 마우스 초기 설정
}

void USettingSystem::GraphicsInitialize()
{
	// TODO : 그래픽 초기 설정
}

void USettingSystem::SetResolution(FIntPoint Resolution)
{
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		Settings->SetScreenResolution(Resolution);
		Settings->SetFullscreenMode(EWindowMode::Windowed);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}

void USettingSystem::SetScreenMode(EWindowMode::Type Mode)
{
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		if (Mode == EWindowMode::Fullscreen)
		{
			FIntPoint DesktopRes = Settings->GetDesktopResolution();
			Settings->SetScreenResolution(DesktopRes);
		}

		Settings->SetFullscreenMode(Mode);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}

void USettingSystem::ResolutionInitialize()
{
	// 전체화면 모드 적용
	if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
	{
		FIntPoint DesktopRes = Settings->GetDesktopResolution();
		Settings->SetScreenResolution(DesktopRes);
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}

void USettingSystem::SetSoundParamsOverride(FName ClassName, float Volume, float Pitch, float FadeInTime)
{
	if (!BaseSoundMix) return;
	UGameplayStatics::SetBaseSoundMix(GetWorld(), BaseSoundMix);

	USoundClass* SoundClass = *SoundClassList.Find(ClassName);
	if (!SoundClass) return;

	UGameplayStatics::SetSoundMixClassOverride(
	GetWorld(),
	BaseSoundMix,
	SoundClass,
	Volume,
	Pitch,
	FadeInTime,
	true);

	UGameplayStatics::PushSoundMixModifier(GetWorld(), BaseSoundMix);
}