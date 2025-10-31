// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingSystem.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API USettingSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

	void AddSoundClassList(USoundClass* SoundClass, FName& ClassName);
	void SetSoundSoundMix(USoundMix* SoundMix);

    // 볼륨 초기설정
	void VolumeInitialize();
    // 마우스 초기설정
	void MouseInitialize();
    // 그래픽 초기설정
	void GraphicsInitialize();
	// 해상도 초기설정
	void ResolutionInitialize();

	UFUNCTION(BlueprintCallable)
	void SetResolution(FIntPoint Resolution);
	UFUNCTION(BlueprintCallable)
	void SetScreenMode(EWindowMode::Type Mode);
	UFUNCTION(BlueprintCallable)
	void SetSoundParamsOverride(FName ClassName, float Volume = 3.0f, float Pitch = 1.0f, float FadeInTime = 1.0f);
protected:

    UPROPERTY()
    TMap<FName, USoundClass*> SoundClassList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundMix* BaseSoundMix;
};
