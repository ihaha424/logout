// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogManager/DialogDatabaseAsset.h"
#include "Blueprint/UserWidget.h"
#include "Log/TPTLog.h"

bool UDialogDatabaseAsset::FindDialogData(TSubclassOf<UUserWidget> WidgetClass, int32 ID, FDialogDataTableBase& OutData) const
{
    const FDialogDataById* PerWidget = Database.Find(WidgetClass);
    if (PerWidget)
    {
        const FName* Name = PerWidget->DataIndexKeyMap.Find(ID);
        if (!Name)
        {
            TPT_LOG(DialogLog, Error, TEXT("DataIndexKeyMap ID: %d is Invaild."), ID);
            return false;
        }
        static const FString Ctx(TEXT("DialogLookup"));
        const FDialogDataTableBase* Found = PerWidget->DataTable->FindRow<FDialogDataTableBase>(*Name, Ctx);
        if (Found)
        {
            OutData = *Found;
            return true;
        }
    }
    return false;
}

bool UDialogDatabaseAsset::FindDialogDataInstancedStruct(TSubclassOf<UUserWidget> WidgetClass, int32 ID, FInstancedStruct& Row) const
{
    const FDialogDataById* PerWidget = Database.Find(WidgetClass);
    if (PerWidget)
    {
        const FName* Name = PerWidget->DataIndexKeyMap.Find(ID);
        if (!Name)
        {
            TPT_LOG(DialogLog, Error, TEXT("DataIndexKeyMap ID: %d is Invaild."), ID);
            return false;
        }
        static const FString Ctx(TEXT("DialogLookup"));

        const UScriptStruct* RowStruct = PerWidget->DataTable->GetRowStruct(); // ← 실제 Row 타입
        // (선택) 예상하는 베이스인지 확인
        ensure(RowStruct && RowStruct->IsChildOf(FDialogDataTableBase::StaticStruct()));

        // 템플릿 없는 조회: Raw 메모리 포인터 획득
        if (uint8* Raw = PerWidget->DataTable->FindRowUnchecked(*Name))          // 또는 GetRowMap()[Name]
        {
            Row.InitializeAs(RowStruct, Raw);
            return true;
        }
    }
    return false;
}

bool UDialogDatabaseAsset::FindDialogDataByIdInstancedStruct(int32 ID, FInstancedStruct& Row, TSubclassOf<UUserWidget>& OutWidgetClass) const
{
    for (const auto& Pair : Database)
    {
        const TSubclassOf<UUserWidget> WClass = Pair.Key;
        const FDialogDataById& PerWidget = Pair.Value;
        const FName* Name = PerWidget.DataIndexKeyMap.Find(ID);
        if (Name)
        {
            static const FString Ctx(TEXT("DialogLookup"));

            const UScriptStruct* RowStruct = PerWidget.DataTable->GetRowStruct(); // ← 실제 Row 타입
            // (선택) 예상하는 베이스인지 확인
            ensure(RowStruct && RowStruct->IsChildOf(FDialogDataTableBase::StaticStruct()));

            // 템플릿 없는 조회: Raw 메모리 포인터 획득
            if (uint8* Raw = PerWidget.DataTable->FindRowUnchecked(*Name))          // 또는 GetRowMap()[Name]
            {
                Row.InitializeAs(RowStruct, Raw);
                OutWidgetClass = WClass;
                return true;
            }
        }
    }
    return false;
}

bool UDialogDatabaseAsset::FindDialogDataById(int32 ID, FDialogDataTableBase& OutData, TSubclassOf<UUserWidget>& OutWidgetClass) const
{
    for (const auto& Pair : Database)
    {
        const TSubclassOf<UUserWidget> WClass = Pair.Key;
        const FDialogDataById& PerWidget = Pair.Value;
        const FName* Name = PerWidget.DataIndexKeyMap.Find(ID);
        if (Name)
        {
            static const FString Ctx(TEXT("DialogLookup"));
            const FDialogDataTableBase* Found = PerWidget.DataTable->FindRow<FDialogDataTableBase>(*Name, Ctx);
            if (Found)
            {
                OutData = *Found;
                OutWidgetClass = WClass;
                return true;
            }
        }
    }
    return false;
}
