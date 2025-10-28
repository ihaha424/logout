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

            uint64 LogicalID = MakeLogicalID(DialogDataTableBase->Level, DialogDataTableBase->ID, DialogDataTableBase->bIsTrigger);
            Datas.DataIndexKeyMap.Add({ LogicalID, TablePair.Key});

            if (DialogDataTableBase->bIsTrigger)
                UDialogTreeBuilder::AddTriggerEventByPath(&DialogNode, DialogDataTableBase->Level, DialogDataTableBase->ID, LogicalID);
            else
                UDialogTreeBuilder::AddLeafByPath(&DialogNode, DialogDataTableBase->Level, DialogDataTableBase->ID, LogicalID);
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

int32 UDialogManager::NextSequence(int32 Jump, bool bExcuteBindEvent)
{
    int32 Sequence = DialogNode.NextSequence(Jump);
    if (bExcuteBindEvent)
    {
        TArray<int32> Level = GetSequence();
        const int32 Last = Level.Pop();
        ExcuteByDialogEvent(Level, Last);
    }
	return Sequence;
}

TArray<int32> UDialogManager::GetSequence() const
{
    return  DialogNode.GetSequence();
}

void UDialogManager::SetSequence(const TArray<int32>& LevelIndex, bool bExcuteBindEvent)
{
    DialogNode.SetSequence(LevelIndex);
}


bool UDialogManager::AddByDialogEvent(const TArray<int32>& Level, int32 Index, UObject* Target, FName FunctionName)
{
    if (!Target || FunctionName.IsNone()) return false;

    FScriptDelegate Delegate;
    Delegate.BindUFunction(Target, FunctionName);

    uint64  LogicalID = MakeLogicalID(Level, Index, false);
    auto& Ev = ExcuteByDialogEventMap.FindOrAdd(LogicalID);
    Ev.Add(Delegate);
    return true;
}

bool UDialogManager::RemoveyDialogEvent(const TArray<int32>& Level, int32 Index, UObject* Target, FName FunctionName)
{
    if (!Target || FunctionName.IsNone()) return false;

    uint64  LogicalID = MakeLogicalID(Level, Index, false);
    if (FExcuteByDialogEvent* Ev = ExcuteByDialogEventMap.Find(LogicalID))
    {
        FScriptDelegate Delegate;
        Delegate.BindUFunction(Target, FunctionName);

        Ev->Remove(Delegate);
        return true;
    }
    return false;
}

bool UDialogManager::ExcuteByDialogEvent(const TArray<int32>& Level, int32 Index)
{
    uint64  LogicalID = MakeLogicalID(Level, Index, false);
    FExcuteByDialogEvent* Event = ExcuteByDialogEventMap.Find(LogicalID);
    if (!Event)
        return false;

    Event->Broadcast(Index);
    return true;
}


int32 UDialogManager::EventTrriger(int32 Level, int32 index) const
{
    return DialogNode.EventTrriger(Level, index);
}

int32 UDialogManager::MakeLogicalID(const TArray<int32> Level, int32 ID, bool bTriggered) const
{
    uint32 H = 0;
    if (Level.Num() > 0)
    {
        const uint32 C = FCrc::MemCrc32(Level.GetData(), Level.Num() * sizeof(int32));
        H = HashCombineFast(H, C);
    }
    else
    {
        H = HashCombineFast(H, 0x9E3779B9u);
    }

    H = HashCombineFast(H, ::GetTypeHash(ID));

    H = HashCombineFast(H, uint32(bTriggered));

    return int32(H);
}
