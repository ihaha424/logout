// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogManager/DialogTreeBuilder.h"
#include "Log/TPTLog.h"

FDialogNode* UDialogTreeBuilder::AddLeafByPath(FDialogNode* Root, const TArray<int32>& InLevelPath, int32 LeafID, bool bUnique, bool bAllowPromoteLeafToInternal)
{
	if (!Root)
    {
        TPT_LOG(DialogLog, Warning, TEXT("AddLeafByPath: Root is null"));
        return nullptr;
    }

    // 경로 길이 0이면 루트 자체를 리프로 사용하려는 의도
    FDialogNode* Target = EnsurePath(Root, InLevelPath, bAllowPromoteLeafToInternal);
    if (!Target) return nullptr;

    // 최종 노드는 리프여야 LeafList에 ID를 넣을 수 있음
    if (!Target->IsLeaf() && Target->NodeList.Num() > 0)
    {
        TPT_LOG(DialogLog, Warning, TEXT("AddLeafByPath: target node has children (not a leaf). Can't add LeafID here."));
        return nullptr;
    }

    if (LeafID < 0)
    {
        TPT_LOG(DialogLog, Warning, TEXT("AddLeafByPath: negative LeafID=%d"), LeafID);
        return nullptr;
    }

    const int32 OldNum = Target->LeafList.Num();
    if (OldNum <= LeafID)
    {
        Target->LeafList.SetNum(LeafID + 1);
        for (int32 i = OldNum; i < Target->LeafList.Num(); ++i) 
            Target->LeafList[i] = -1;
    }
    Target->LeafList[LeafID] = LeafID;

    // 선택 인덱스 보정(필요 시)
    EnsureSequenceValid(*Target);

    return Target;
}

FDialogNode* UDialogTreeBuilder::AddTriggerEventByPath(FDialogNode* Root, const TArray<int32>& InLevelPath, int32 TriggerKey, bool bUnique, bool bAllowPromoteLeafToInternal)
{
    if (!Root)
    {
        TPT_LOG(DialogLog, Warning, TEXT("AddTriggerEventByPath: Root is null"));
        return nullptr;
    }

    // 트리 노드 확보(없으면 생성, 리프 아래로 내려가야 하면 승격 여부는 옵션)
    FDialogNode* Target = EnsurePath(Root, InLevelPath, bAllowPromoteLeafToInternal);
    if (!Target) return nullptr;

    // Trigger는 리프/비-리프 모두 가능 (제약 없음)
    if (TriggerKey < 0)
    {
        TPT_LOG(DialogLog, Warning, TEXT("AddLeafByPath: negative TriggerKey=%d"), TriggerKey);
        return nullptr;
    }

    const int32 OldNum = Target->LeafList.Num();
    if (OldNum <= TriggerKey)
    {
        Target->LeafList.SetNum(TriggerKey + 1);
        for (int32 i = OldNum; i < Target->LeafList.Num(); ++i)
            Target->LeafList[i] = -1;
    }
    Target->LeafList[TriggerKey] = TriggerKey;

    // 선택 인덱스 보정(필요 시)
    EnsureSequenceValid(*Target);
    return Target;
}

FDialogNode* UDialogTreeBuilder::EnsurePath(FDialogNode* Root, const TArray<int32>& Path, bool bAllowPromoteLeafToInternal)
{
    if (!Root) return nullptr;

    FDialogNode* Cur = Root;
    EnsureSequenceValid(*Cur); // 루트 보정

    // 루트의 ParentPath는 빈 배열(원하면 강제 초기화)
    // if (Cur->LevelPath.Num() != 0) Cur->LevelPath.Reset();

    for (int32 depth = 0; depth < Path.Num(); ++depth)
    {
        const int32 Idx = Path[depth];
        if (Idx < 0)
        {
            TPT_LOG(DialogLog, Warning, TEXT("EnsurePath: negative index at depth=%d"), depth);
            return nullptr;
        }

        // 리프(LeafList 보유) 아래로 더 내려가려는 경우
        if (Cur->IsLeaf() && Cur->LeafList.Num() > 0 && depth < Path.Num() - 1)
        {
            if (!bAllowPromoteLeafToInternal)
            {
                TPT_LOG(DialogLog, Warning, TEXT("EnsurePath: cannot descend under a leaf at depth=%d (path conflict)"), depth);
                return nullptr;
            }

            // 승격: 기존 리프 ID들을 자식[0]으로 이동
            const int32 OldLeafCount = Cur->LeafList.Num();

            Cur->NodeList.SetNum(1);              // 자식 0 생성(값 타입이라 디폴트 생성됨)
            EnsureLevelPath(Cur->NodeList[0], Cur->LevelPath, 0);
            Cur->NodeList[0].LeafList = MoveTemp(Cur->LeafList); // 리프 ID 이전
            EnsureSequenceValid(Cur->NodeList[0]);               // 자식 리프 보정

            Cur->LeafList.Reset();                // 현재 노드는 내부 노드화
            EnsureSequenceValid(*Cur);

            TPT_LOG(DialogLog, Log, TEXT("EnsurePath: promoted leaf→internal at depth=%d (moved %d IDs to child[0])"),
                depth, OldLeafCount);
        }

        // 자식 배열 크기 보장 & 새 요소 LevelPath 세팅
        const int32 OldNum = Cur->NodeList.Num();
        if (OldNum <= Idx)
        {
            Cur->NodeList.SetNum(Idx + 1);
            for (int32 i = OldNum; i <= Idx; ++i)
            {
                EnsureLevelPath(Cur->NodeList[i], Cur->LevelPath, i);
                EnsureSequenceValid(Cur->NodeList[i]);
            }
        }
        else
        {
            // 기존 자식도 경로가 비어있다면 한 번 세팅(안전)
            if (Cur->NodeList[Idx].LevelPath.Num() == 0 && !(Cur->LevelPath.Num() == 0 && Idx == 0))
            {
                EnsureLevelPath(Cur->NodeList[Idx], Cur->LevelPath, Idx);
            }
            EnsureSequenceValid(Cur->NodeList[Idx]);
        }

        // 다음 단계로 진행
        Cur = &Cur->NodeList[Idx];
    }

    return Cur; // 최종 노드
}

void UDialogTreeBuilder::EnsureLevelPath(FDialogNode& Node, const TArray<int32>& ParentPath, int32 MyIndex)
{
	Node.LevelPath = ParentPath;
	Node.LevelPath.Add(MyIndex);
}

void UDialogTreeBuilder::EnsureSequenceValid(FDialogNode& Node)
{
	const int32 Size = Node.SizeForSequence();
	if (Size <= 0) { Node.Sequence = -1; return; }
	if (Node.Sequence < 0 || Node.Sequence >= Size) Node.Sequence = 0;
}
