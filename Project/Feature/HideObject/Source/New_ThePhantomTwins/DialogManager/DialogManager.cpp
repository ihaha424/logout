// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogManager/DialogManager.h"
#include "Blueprint/UserWidget.h"
#include "DialogManager/DialogTreeBuilder.h"

#include "Log/TPTLog.h"

/**
 * @brief  UDialogManager
 */

void UDialogManager::Initialize(TMap<TSubclassOf<UUserWidget>, TObjectPtr<UDataTable>>& InitializeData)
{
    DataBase = NewObject<UDialogDatabaseAsset>(this);
    for (const auto& Pair : InitializeData)
    {
        const TSubclassOf<UUserWidget>& WClass = Pair.Key;
        UDataTable* Table = Pair.Value;
        if (!Table->GetRowStruct()->IsChildOf(FDialogDataTableBase::StaticStruct()))
        {
            TPT_LOG(DialogLog, Error, TEXT("Data is not ChildOf FDialogDataTableBase."));
            return;
        }

        FDialogDataById& Datas = DataBase->Database.FindOrAdd(WClass);
        Datas.WidgetClass = WClass;
        Datas.DataTable = Table;
        for (const auto& TablePair : Table->GetRowMap())
        {
            uint8* pointer = TablePair.Value;
            FDialogDataTableBase* DialogDataTableBase = reinterpret_cast<FDialogDataTableBase*>(pointer);
            Datas.DataIndexKeyMap.Add({ DialogDataTableBase->ID, TablePair.Key });

            if (DialogDataTableBase->bIsTrigger)
                UDialogTreeBuilder::AddTriggerEventByPath(&DialogNode, DialogDataTableBase->Level, DialogDataTableBase->ID);
            else
                UDialogTreeBuilder::AddLeafByPath(&DialogNode, DialogDataTableBase->Level, DialogDataTableBase->ID);
        }
    }

    //TArray<int32> CurSequnce = DialogNode.GetSequence();
    //int32 A = DialogNode.NextSequence(5);
    //A = DialogNode.GetSequenceID();

    //TArray<int32> CurSequnce2 = DialogNode.GetSequence();
    //A = DialogNode.GetSequenceID();

    //FDialogDataTableBase Data;
    //TSubclassOf<UUserWidget> WClass;
    //bool bHit = DataBase->FindDialogDataById(A, Data, WClass);

    // EventTrriger(0,0);

}

int32 UDialogManager::NextSequence(int32 Jump)
{
	return DialogNode.NextSequence(Jump);
}

int32 UDialogManager::EventTrriger(int32 Level, int32 index) const
{
    return DialogNode.EventTrriger(Level, index);
}

bool UDialogManager::AddByDialogEvent(int32 Index, UObject* Target, FName FunctionName)
{
    if (!Target || FunctionName.IsNone()) return false;

    FScriptDelegate Delegate;
    Delegate.BindUFunction(Target, FunctionName);

    auto& Ev = ExcuteByDialogEventMap.FindOrAdd(Index);
    Ev.Add(Delegate);
    return true;
}

bool UDialogManager::RemoveyDialogEvent(int32 Index, UObject* Target, FName FunctionName)
{
    if (!Target || FunctionName.IsNone()) return false;

    if (FExcuteByDialogEvent* Ev = ExcuteByDialogEventMap.Find(Index))
    {
        FScriptDelegate Delegate;
        Delegate.BindUFunction(Target, FunctionName);

        Ev->Remove(Delegate);
        return true;
    }
    return false;
}

bool UDialogManager::ExcuteByDialogEvent(int32 Index)
{
    FExcuteByDialogEvent* Event = ExcuteByDialogEventMap.Find(Index);
    if (!Event)
        return false;

    Event->Broadcast(Index);
    return true;
}

TArray<int32> UDialogManager::GetSequence() const
{
	return  DialogNode.GetSequence();
}

void UDialogManager::SetSequence(const TArray<int32>& LevelIndex)
{
    DialogNode.SetSequence(LevelIndex);
}