// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_QuestionBox.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_QuestionBox::UGA_QuestionBox()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_QuestionBox::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TPT_LOG(GALog, Log, TEXT("UGA_QuestionBox::ActivateAbility"));

// 0) 예외 처리

	if (!HasAuthority(&ActivationInfo))
	{
		// 서버에서만 처리하도록 보장 (또는 서버 RPC 호출)
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* AvatarActor = ActorInfo && ActorInfo->AvatarActor.IsValid() ? ActorInfo->AvatarActor.Get() : nullptr;
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_QuestionBox::ActivateAbility - ItemDataTable not set"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

// 1) DataTable에서 뽑을 수 있는 행들을 수집
	TArray<FRandomDT*> Rows;
	GatherPickableRows(Rows);


// 2) 가중치로 하나 선택
	FRandomDT* Selected = SelectWeightedRandomRow(Rows);


// 3) 선택된 항목 처리 (None == 꽝 포함)
	bool bProcessed = ProcessSelectedRow(AvatarActor, Selected);

	// 후처리 / 로그
	if (!bProcessed)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_QuestionBox::ActivateAbility - ProcessSelectedRow failed"));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


/** 데이터테이블에서 뽑을 수 있는 행들 수집.
 *  필요하다면 ItemType으로 필터링(예: QuestionBox 타입 제외) 추가.
 */
void UGA_QuestionBox::GatherPickableRows(TArray<FRandomDT*>& OutRows) const
{
	OutRows.Reset();
	if (!ItemDataTable) return;

	const TArray<FName> RowNames = ItemDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FRandomDT* Row = ItemDataTable->FindRow<FRandomDT>(RowName, TEXT("UGA_QuestionBox"));
		if (Row)
		{
			// 예: 물음표박스 자체(QuestionBox) 행은 제외
			if (Row->ItemType == EItemType::None) continue;
			if (Row->ItemType == EItemType::AuraDetector) continue;
			if (Row->ItemType == EItemType::QuestionBox) continue;

			// 랜덤 대상으로 포함시키려면 RandomProbability >= 0 등을 체크
			OutRows.Add(Row);
		}
	}
}


/** 가중치 선택. RandomProbability 를 가중치로 사용.
 *  모든 가중치가 0이면 nullptr 반환(즉 꽝 처리는 호출자에서 함).
 */
FRandomDT* UGA_QuestionBox::SelectWeightedRandomRow(const TArray<FRandomDT*>& Rows) const
{
	int32 TotalWeight = 0;
	for (const FRandomDT* R : Rows)
	{
		TotalWeight += FMath::Max(0, R->RandomProbability);
	}

	if (TotalWeight <= 0)
	{
		return nullptr;
	}

	int32 Roll = FMath::RandRange(1, TotalWeight);
	int32 Accum = 0;
	for (FRandomDT* R : Rows)
	{
		Accum += FMath::Max(0, R->RandomProbability);
		if (Roll <= Accum)
			return R;
	}

	return nullptr;
}


/** 선택된 행을 처리:
 *  - ItemType == None => 꽝
 *  - 그 외 => GenerateCount 만큼 인벤토리로 넣기 시도
 */
bool UGA_QuestionBox::ProcessSelectedRow(AActor* AvatarActor, FRandomDT* SelectedRow)
{
	if (!AvatarActor || !SelectedRow) return false;

	// 선택된 아이템 타입을 문자열로 변환하여 로그 출력
	FString ItemTypeString = UEnum::GetValueAsString(SelectedRow->ItemType);
	TPT_LOG(GALog, Log, TEXT("UGA_QuestionBox::ProcessSelectedRow - 선택된 아이템 : %s (Probability: %d)"),
		*ItemTypeString, SelectedRow->RandomProbability);

	// None(꽝) 처리
	if (SelectedRow->ItemType == EItemType::Miss)
	{
		TPT_LOG(GALog, Log, TEXT("UGA_QuestionBox::ProcessSelectedRow - Got 'None' (Miss/꽝)"));
		// 꽝 처리: 클라이언트에 알림, SFX 재생 등

		return true;
	}

	// GenerateCount 가 없으면 기본 1 사용
	int32 SpawnCount = 1;
#if WITH_EDITOR
	// 가정: FItemDataTable에 GenerateCount가 있으면 사용
	// SpawnCount = SelectedRow->GenerateCount > 0 ? SelectedRow->GenerateCount : 1;
#endif

	// 인벤토리에 추가 시도
	bool bAdded = AddItemToInventory(AvatarActor, SelectedRow->ItemType, SpawnCount, *SelectedRow);

	if (bAdded)
	{
		//NotifyClient_ItemAcquired(AvatarActor, *SelectedRow, SpawnCount);
	}
	else
	{
		// 인벤토리 부족 등 실패 시 처리: 월드 드롭 / 메시지 등
		// (예: SpawnActor to drop item in world)
		UE_LOG(LogTemp, Warning, TEXT("UGA_QuestionBox::ProcessSelectedRow - failed to add to inventory. Consider dropping to world."));
	}

	return true;
}

bool UGA_QuestionBox::AddItemToInventory(AActor* AvatarActor, EItemType ItemType, int32 Quantity, const FRandomDT& RowData)
{
	return true;
}
