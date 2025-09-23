// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FDialogNode
{
	// -1: Inactive, >=0: 현재 선택 인덱스
	TArray<int32> LevelPath;
	int32 Sequence = -1;       // 현재 선택 인덱스 (비활성:-1)

	TArray<FDialogNode>	NodeList;  // 자식 노드(비-리프에서만 사용)
	TArray<int32>	LeafList;   // 리프의 후보 ID들(리프에서만 사용)
	TArray<int32>	TriggerList;// 이벤트 키(리프/노드 모두 가능)

	// --- Public API ---
	int32 NextSequence(int32 Jump = 1);
	void  SetSequence(const TArray<int32>& LevelIndex);
	TArray<int32> GetSequence() const;
	int32 GetSequenceID() const;
	/**
	 * @brief : 현재 Sequence의 Event Trigger
	 * @param DepthLevel : 현재 실행중인 Sequence의 Depth 
	 * @param Index : 인덱스 번호
	 * @return : Trigger ID 값
	 */
	int32 EventTrriger(int32 DepthLevel, int32 Index) const;

	// --- (선택) 디버그 보조 ---
	bool  IsLeaf() const { return NodeList.Num() == 0; }
	int32 SizeForSequence() const { return IsLeaf() ? LeafList.Num() : NodeList.Num(); }

private:
	// 헬퍼들
	static void  BuildActivePath(FDialogNode* Root, TArray<FDialogNode*>& OutPath /*root..leaf*/);
	static void  BuildActivePathConst(const FDialogNode* Root, TArray<const FDialogNode*>& OutPath /*root..leaf*/);
	static void  NormalizeOrInit(FDialogNode* N);
	static bool  ValidateSetPath(const FDialogNode* Root, const TArray<int32>& LevelIndex);
	static void  ApplySetPath(FDialogNode* Root, const TArray<int32>& LevelIndex);
	static bool  DescendZeroUntilLeaf(FDialogNode* N); // 가능한 경우 0으로 내려가 leaf까지 경로 설정
};
