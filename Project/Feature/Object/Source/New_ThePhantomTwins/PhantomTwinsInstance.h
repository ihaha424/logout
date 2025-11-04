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
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingSystem | Sound")
    TMap<FName, USoundClass*> SoundClassList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingSystem | Sound")
    USoundMix* BaseSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogManager")
	TObjectPtr<UDialogManager> DialogManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UUserWidget>, TObjectPtr<UDataTable>> Initialize;
};
