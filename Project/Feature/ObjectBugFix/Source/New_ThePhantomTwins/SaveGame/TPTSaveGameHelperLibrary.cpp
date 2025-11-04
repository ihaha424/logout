// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/TPTSaveGameHelperLibrary.h"

DEFINE_LOG_CATEGORY(LogTPTSaveGame);

static bool IsTPTSaveGameClass(UClass* Cls)
{
	return Cls && (Cls->IsChildOf(UTPTSaveGame::StaticClass()) ||
		Cls->IsChildOf(UTPTLocalPlayerSaveGame::StaticClass()));
}

USaveGame* UTPTSaveGameHelperLibrary::GetSaveGameData_BP(
    TSubclassOf<USaveGame> SaveGameClass, 
    const FString& SlotName, 
    int32 UserIndex, 
    bool bCreateIfMissing)
{
    if (!IsTPTSaveGameClass(*SaveGameClass))
    {
        UE_LOG(LogTPTSaveGame, Warning, TEXT("Invalid SaveGameClass: %s"), *GetNameSafe(*SaveGameClass));
        return nullptr;
    }

    const FString FullSlot = SaveGameClass->GetName() + SlotName;

    USaveGame* Obj = UGameplayStatics::LoadGameFromSlot(FullSlot, UserIndex);
    if (!Obj && bCreateIfMissing)
    {
        Obj = UGameplayStatics::CreateSaveGameObject(*SaveGameClass);
        if (!Obj)
        {
            UE_LOG(LogTPTSaveGame, Warning, TEXT("CreateSaveGameObject failed: %s"), *GetNameSafe(*SaveGameClass));
        }
    }
    return Obj;
}

bool UTPTSaveGameHelperLibrary::SetSaveGameData_BP(USaveGame* Data, 
    const FString& SlotName, 
    int32 UserIndex)
{
    if (!Data || !IsTPTSaveGameClass(Data->GetClass()))
    {
        UE_LOG(LogTPTSaveGame, Warning, TEXT("SetSaveGameData_BP: invalid Data"));
        return false;
    }

    const FString FullSlot = Data->GetClass()->GetName() + SlotName;
    const bool bOK = UGameplayStatics::SaveGameToSlot(Data, FullSlot, UserIndex);
    if (!bOK)
    {
        UE_LOG(LogTPTSaveGame, Warning, TEXT("SaveGameToSlot failed: %s"), *FullSlot);
    }
    return bOK;
}