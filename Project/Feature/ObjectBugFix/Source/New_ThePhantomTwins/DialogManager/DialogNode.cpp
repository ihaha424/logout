// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogManager/DialogNode.h"
#include "Log/TPTLog.h"

// 현재 활성 경로(Sequence가 -1이면 0으로 초기화하며 가능한 한 leaf까지) 생성
void FDialogNode::NormalizeOrInit(FDialogNode* N)
{
	if (!N) return;
	const int32 Size = N->SizeForSequence();
	if (Size <= 0) { N->Sequence = -1; return; }
	if (N->Sequence < 0 || N->Sequence >= Size) N->Sequence = 0;
	if (!N->IsLeaf())
	{
		NormalizeOrInit(&N->NodeList[N->Sequence]);
	}
}

// root→leaf 포인터 경로(비정상값은 가능한 한 0으로 맞추며 leaf까지)
void FDialogNode::BuildActivePath(FDialogNode* Root, TArray<FDialogNode*>& OutPath)
{
	OutPath.Reset();
	if (!Root) return;

	FDialogNode* Cur = Root;
	while (Cur)
	{
		NormalizeOrInit(Cur);
		OutPath.Add(Cur);

		if (Cur->IsLeaf()) break;

		const int32 Size = Cur->NodeList.Num();
		if (Size <= 0 || Cur->Sequence < 0 || Cur->Sequence >= Size) break;
		Cur = &Cur->NodeList[Cur->Sequence];
	}
}

void FDialogNode::BuildActivePathConst(const FDialogNode* Root, TArray<const FDialogNode*>& OutPath)
{
	OutPath.Reset();
	if (!Root) return;

	const FDialogNode* Cur = Root;
	while (Cur)
	{
		OutPath.Add(Cur);
		if (Cur->IsLeaf()) break;

		const int32 Size = Cur->NodeList.Num();
		if (Cur->Sequence < 0 || Cur->Sequence >= Size) break;
		Cur = &Cur->NodeList[Cur->Sequence];
	}
}

// SetSequence: 검증(변경 없음) → 적용(정상 시만)
bool FDialogNode::ValidateSetPath(const FDialogNode* Root, const TArray<int32>& LevelIndex)
{
	const FDialogNode* Cur = Root;
	int32 Depth = 0;
	for (Depth = 0; Depth < LevelIndex.Num(); ++Depth)
	{
		if (!Cur) return false;
		const int32 Want = LevelIndex[Depth];

		if (Cur->IsLeaf())
		{
			// 리프: LeafList 인덱스
			if (Want < 0 || Want >= Cur->LeafList.Num())
			{
				TPT_LOG(DialogLog, Warning, TEXT("SetSequence invalid leaf index depth=%d want=%d size=%d"),
					Depth, Want, Cur->LeafList.Num());
				return false;
			}
			// 더 깊이 지정이 들어오면 안 되지만 들어와도 아래 로직이 막아줌
			return Depth + 1 == LevelIndex.Num();
		}
		else
		{
			// 비-리프: NodeList 인덱스
			if (Want < 0 || Want >= Cur->NodeList.Num())
			{
				TPT_LOG(DialogLog, Warning, TEXT("SetSequence invalid node index depth=%d want=%d size=%d"),
					Depth, Want, Cur->NodeList.Num());
				return false;
			}
			Cur = &Cur->NodeList[Want];
		}
	}

	// 남은 깊이는 모두 0으로 내려갈 수 있어야 함
	while (Cur && !Cur->IsLeaf())
	{
		if (Cur->NodeList.Num() <= 0)
		{
			TPT_LOG(DialogLog, Warning, TEXT("SetSequence cannot descend at depth=%d (no children)"), Depth);
			return false;
		}
		Cur = &Cur->NodeList[0];
	}
	// 마지막이 리프면 0이 유효해야 함
	if (Cur && Cur->IsLeaf() && Cur->LeafList.Num() <= 0)
	{
		TPT_LOG(DialogLog, Warning, TEXT("SetSequence leaf has no LeafList"));
		return false;
	}
	return true;
}

void FDialogNode::ApplySetPath(FDialogNode* Root, const TArray<int32>& LevelIndex)
{
	FDialogNode* Cur = Root;
	for (int32 Depth = 0; Depth < LevelIndex.Num(); ++Depth)
	{
		const int32 Want = LevelIndex[Depth];
		Cur->Sequence = Want;
		if (Cur->IsLeaf()) return; // 여기서 끝

		Cur = &Cur->NodeList[Want];
	}

	// 남은 깊이는 0으로 내리며 leaf까지
	while (Cur && !Cur->IsLeaf())
	{
		Cur->Sequence = 0;
		Cur = &Cur->NodeList[0];
	}
	// leaf면 0으로
	if (Cur && Cur->IsLeaf())
	{
		Cur->Sequence = 0;
	}
}

bool FDialogNode::DescendZeroUntilLeaf(FDialogNode* N)
{
	if (!N) return false;
	if (N->IsLeaf())
	{
		if (N->LeafList.Num() <= 0) { N->Sequence = -1; return false; }
		N->Sequence = 0;
		return true;
	}
	if (N->NodeList.Num() <= 0) { N->Sequence = -1; return false; }
	N->Sequence = 0;
	return DescendZeroUntilLeaf(&N->NodeList[0]);
}

// --- Public API ---

void FDialogNode::SetSequence(const TArray<int32>& LevelIndex)
{
	if (!ValidateSetPath(this, LevelIndex))
	{
		TPT_LOG(DialogLog, Warning, TEXT("SetSequence aborted: invalid LevelIndex"));
		return;
	}
	ApplySetPath(this, LevelIndex);
}

TArray<int32> FDialogNode::GetSequence() const
{
	TArray<int32> Out;
	const FDialogNode* Cur = this;
	while (Cur)
	{
		if (Cur->Sequence < 0) break; // 비활성 구간에서 중단
		const int32 Size = Cur->SizeForSequence();
		if (Cur->Sequence >= Size) break; // 비정상 값

		Out.Add(Cur->Sequence);

		if (Cur->IsLeaf()) break;
		Cur = &Cur->NodeList[Cur->Sequence];
	}
	return Out;
}

int32 FDialogNode::GetSequenceID() const
{
	TArray<const FDialogNode*> Path;
	BuildActivePathConst(this, Path);
	if (Path.Num() == 0) return -1;

	const FDialogNode* Leaf = Path.Last();
	if (!Leaf->IsLeaf()) return -1;
	if (Leaf->Sequence < 0 || Leaf->Sequence >= Leaf->LeafList.Num()) return -1;
	return Leaf->LeafList[Leaf->Sequence];
}

int32 FDialogNode::NextSequence(int32 Jump /*=1*/)
{
	if (Jump <= 0) return GetSequenceID();

	// 현재 활성 경로를 만든다(가능한 한 0으로 초기화하며 leaf까지)
	TArray<FDialogNode*> Path; // root .. leaf
	BuildActivePath(this, Path);
	if (Path.Num() == 0) return -1;

	// Path 마지막은 가능한 경우 leaf가 되도록 맞춰져 있음
	if (!Path.Last()->IsLeaf())
	{
		// leaf까지 더 못 내려가면 진행 불가
		return -1;
	}

	// 오도미터 방식으로 Jump번 증가
	for (int32 step = 0; step < Jump; ++step)
	{
		int32 carryLevel = Path.Num() - 1; // leaf부터 시작
		bool Carried = true;

		while (Carried)
		{
			FDialogNode* N = Path[carryLevel];
			const int32 Size = N->SizeForSequence();
			if (Size <= 0) return -1;

			N->Sequence += 1;

			if (N->Sequence < Size)
			{
				// 증가 성공 → 아래는 모두 0으로 내리며 leaf까지 다시 만든다
				for (int32 d = carryLevel + 1; d < Path.Num(); ++d)
				{
					DescendZeroUntilLeaf(Path[d]);
				}

				// carryLevel 아래 경로를 다시 재구성(leaf까지)
				{
					TArray<FDialogNode*> NewPath;
					BuildActivePath(this, NewPath);
					Path = MoveTemp(NewPath);
				}
				Carried = false; // 이번 step 완료
			}
			else
			{
				// 넘침 → 현재 레벨 0으로 두고 상위로 캐리
				N->Sequence = 0;
				carryLevel -= 1;
				if (carryLevel < 0)
				{
					// 루트도 넘침 → 더 이상 진행 불가
					return -1;
				}
			}
		}
	}

	return GetSequenceID();
}

int32 FDialogNode::EventTrriger(int32 DepthLevel, int32 Index) const
{
	if (DepthLevel < 0) return -1;

	TArray<const FDialogNode*> Path;
	BuildActivePathConst(this, Path);
	if (Path.Num() == 0) return -1;

	const int32 TargetDepth = FMath::Clamp(DepthLevel, 0, Path.Num() - 1);
	const FDialogNode* Node = Path[TargetDepth];
	if (!Node) return -1;

	if (!Node->TriggerList.IsValidIndex(Index)) return -1;
	return Node->TriggerList[Index];
}
