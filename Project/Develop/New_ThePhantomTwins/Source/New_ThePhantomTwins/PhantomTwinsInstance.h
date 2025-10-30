// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveGame/TPTSaveGame.h"
#include "SaveGame/TPTLocalPlayerSaveGame.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "PhantomTwinsInstance.generated.h"

class UDialogManager;

UCLASS()
class NEW_THEPHANTOMTWINS_API UPhantomTwinsInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	void InitSound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundMix* DuckSoundMix;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundClass* MasterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundClass* MusicClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundClass* SFXClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundClass* CharacterVoiceClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundClass* EnemyVoiceClass;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogManager")
	TObjectPtr<UDialogManager> DialogManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UUserWidget>, TObjectPtr<UDataTable>> Initialize;
};
