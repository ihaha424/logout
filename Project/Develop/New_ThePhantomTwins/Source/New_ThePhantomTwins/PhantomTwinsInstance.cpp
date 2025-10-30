// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"

#include "AudioDevice.h"
#include "Tags/TPTGameplayTags.h"
#include "GameFramework/GameUserSettings.h"
#include "Log/TPTLog.h"
#include "Kismet/GameplayStatics.h"
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

void UPhantomTwinsInstance::InitSound()
{
    if (!DuckSoundMix)
    {
        UE_LOG(LogTemp, Warning, TEXT("DuckSoundMix not assigned."));
        return;
    }

	// 사운드 믹스를 활성화
	UGameplayStatics::SetBaseSoundMix(GetWorld(), DuckSoundMix);
	
	// 각각의 사운드 클래스에 오버라이드 적용
	auto ApplyVolume = [&](USoundClass* SoundClass, float Volume)
	    {
	        if (SoundClass)
	        {
	            UGameplayStatics::SetSoundMixClassOverride(
	                GetWorld(),
	                DuckSoundMix,
	                SoundClass,
	                Volume,
	                1.0f,        // Pitch
	                1.0f,        // FadeInTime
	                true         // ApplyToChildren
	            );
	        }
	    };
	
	ApplyVolume(MasterClass, 1.0f);
	ApplyVolume(MusicClass, 0.8f);
	ApplyVolume(SFXClass, 0.8f);
	ApplyVolume(CharacterVoiceClass, 1.0f);
	ApplyVolume(EnemyVoiceClass, 1.0f);
	
	// 변경사항을 커밋 (적용)
	UGameplayStatics::PushSoundMixModifier(GetWorld(), DuckSoundMix);
}
