// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/TPTSaveGame.h"
#include "SaveGame/TPTLocalPlayerSaveGame.h"
#include "TPTSaveGameHelperLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTPTSaveGame, Log, All);

template<typename T>
concept TPTSaveGameConcept = std::is_base_of_v<UTPTSaveGame, T> || std::is_base_of_v<UTPTLocalPlayerSaveGame, T>;

UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTSaveGameHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	template<TPTSaveGameConcept T>
	static void SetSaveGameData(T* Data, const FString& SlotName = "MainSlot", int32 Slot = 0)
	{
		if (!UGameplayStatics::SaveGameToSlot(Data, FString(T::StaticClass()->GetName() + SlotName), Slot))
		{
			UE_LOG(LogTPTSaveGame, Warning, TEXT("TPTSaveGame set Fail."));
		}
	}

	template<TPTSaveGameConcept T>
	static T* GetSaveGameData(const FString& SlotName = "MainSlot", int32 Slot = 0, bool bCreate = false)
	{
		T* SaveGameInstance = Cast<T>(UGameplayStatics::LoadGameFromSlot(FString(T::StaticClass()->GetName() + SlotName), Slot));
		if (nullptr == SaveGameInstance || bCreate)
		{
			SaveGameInstance = Cast<T>(UGameplayStatics::CreateSaveGameObject(T::StaticClass()));
		}

		if (nullptr == SaveGameInstance)
		{
			UE_LOG(LogTPTSaveGame, Warning, TEXT("TPTSaveGame get Fail."));
		}

		return SaveGameInstance;
	}
};