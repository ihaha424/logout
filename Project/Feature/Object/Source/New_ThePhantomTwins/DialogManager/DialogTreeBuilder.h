// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogManager/DialogNode.h"

class UDialogTreeBuilder
{
public:
    // LevelPath 경로에 해당하는 리프에 ID 추가
    static FDialogNode* AddLeafByPath(FDialogNode* Root, const TArray<int32>& InLevelPath, int32 LeafID, bool bUnique = true, bool bAllowPromoteLeafToInternal = false);
    static FDialogNode* AddTriggerEventByPath(FDialogNode* Root, const TArray<int32>& InLevelPath, int32 TriggerKey, bool bUnique = true, bool bAllowPromoteLeafToInternal = false);


private:
    static FDialogNode* EnsurePath(FDialogNode* Root, const TArray<int32>& Path, bool bAllowPromoteLeafToInternal);
    static void         EnsureLevelPath(FDialogNode& Node, const TArray<int32>& ParentPath, int32 MyIndex);
    static void         EnsureSequenceValid(FDialogNode& Node);
};