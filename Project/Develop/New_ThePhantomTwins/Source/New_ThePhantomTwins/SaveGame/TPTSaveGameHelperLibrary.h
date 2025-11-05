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
	UFUNCTION(BlueprintCallable, Category = "TPT|SaveGame",
		meta = (DeterminesOutputType = "SaveGameClass"))
	static USaveGame* GetSaveGameData_BP(TSubclassOf<USaveGame> SaveGameClass,
		const FString& SlotName = TEXT("MainSlot"),
		int32 UserIndex = 0,
		bool bCreateIfMissing = true
	);

	UFUNCTION(BlueprintCallable, Category = "TPT|SaveGame")
	static bool SetSaveGameData_BP(USaveGame* Data,
		const FString& SlotName = TEXT("MainSlot"),
		int32 UserIndex = 0
	);

	template<TPTSaveGameConcept T>
	static bool SetSaveGameData(T* Data, const FString& SlotName = "MainSlot", int32 Slot = 0)
	{
		if (!UGameplayStatics::SaveGameToSlot(Data, FString(T::StaticClass()->GetName() + SlotName), Slot))
		{
			UE_LOG(LogTPTSaveGame, Warning, TEXT("TPTSaveGame set Fail."));
			return false;
		}
		return true;
	}

	template<TPTSaveGameConcept T>
	static bool DeleteSaveGameData(T* Data, const FString& SlotName = "MainSlot", int32 Slot = 0)
	{
		if (!UGameplayStatics::DeleteGameInSlot(FString(T::StaticClass()->GetName() + SlotName), Slot))
		{
			UE_LOG(LogTPTSaveGame, Warning, TEXT("TPTSaveGame delete Fail."));
			return false;
		}
		return true;
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