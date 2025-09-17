#include "GA_QuestionBox.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Objects/InventoryComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_QuestionBox::UGA_QuestionBox()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_QuestionBox::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	PS = Character->GetPS();
	NULLCHECK_CODE_RETURN_LOG(PS, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	PC = Cast<APC_Player>(ActorInfo->PlayerController.Get());


	// 서버 전용 처리
	//if (!HasAuthority(&ActivationInfo))
	//{
	//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	//	return;
	//}

	// 아바타 유효성 체크
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor || !ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_QuestionBox::ActivateAbility - Invalid AvatarActor or ItemDataTable"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 1) DataTable에서 뽑을 행 수집
	TArray<FRandomDT*> Rows;
	GatherPickableRows(Rows);

	// 2) 가중치 기반 랜덤 선택
	FRandomDT* Selected = SelectWeightedRandomRow(Rows);


	if (PC && Selected)
	{
		// 3) 인벤토리 추가
		AddItemToInventory(Selected->ItemType, Selected->GenerateCount);

		// 4) 선택된 아이템 UI 표시를 클라이언트로 보내기
		S2C_ShowQuestionBoxWidget(*Selected);
	}


	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_QuestionBox::GatherPickableRows(TArray<FRandomDT*>& OutRows) const
{
	OutRows.Reset();
	if (!ItemDataTable) return;

	for (const FName& RowName : ItemDataTable->GetRowNames())
	{
		if (FRandomDT* Row = ItemDataTable->FindRow<FRandomDT>(RowName, TEXT("UGA_QuestionBox")))
		{
			// 제외할 타입 필터링
			if (Row->ItemType == EItemType::None ||
				Row->ItemType == EItemType::AuraDetector ||
				Row->ItemType == EItemType::QuestionBox)
			{
				continue;
			}
			OutRows.Add(Row);
		}
	}
}

FRandomDT* UGA_QuestionBox::SelectWeightedRandomRow(const TArray<FRandomDT*>& Rows) const
{
	int32 TotalWeight = 0;
	for (const FRandomDT* R : Rows)
	{
		TotalWeight += FMath::Max(0, R->RandomProbability);
	}
	if (TotalWeight <= 0) return nullptr;

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

void UGA_QuestionBox::AddItemToInventory(EItemType ItemType, int32 Quantity)
{
	if (!PS || !PS->InventoryComp) return;

	switch (ItemType)
	{
	case EItemType::NoiseBomb:
	case EItemType::EMP:
	case EItemType::HealPack:
	case EItemType::MentalPack:
	case EItemType::Key:
	case EItemType::Navigation:
		for (int32 i = 0; i < Quantity; i++)
		{
			PS->InventoryComp->AddItem(ItemType);
		}
		break;
	default:
		break;
	}
}

void UGA_QuestionBox::S2C_ShowQuestionBoxWidget_Implementation(const FRandomDT& SelectedRow)
{
	if (!PC) return;

	if (PC)
	{
		FString PCName = PC->GetName(); // 보통 "PC_0", "PC_1" 이런 식
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Cyan,
				FString::Printf(TEXT(" S2C_ShowQuestionBoxWidget : %s (%s)"),
					Character->HasAuthority() ? TEXT("Host(Server)") : TEXT("Guest(Client)"),
					*PCName)
			);
		}
	}

	// 클라이언트에서 UI 위젯을 설정하는 함수 호출
	SetQuestionBoxWidget(const_cast<FRandomDT*>(&SelectedRow));
}

void UGA_QuestionBox::SetQuestionBoxWidget(FRandomDT* SelectedRow)
{
	UQuestionBoxTextWidget* QuestionBoxTextWidget = Cast<UQuestionBoxTextWidget>(
		PC->GetWidget(TEXT("QuestionBoxText")));
	if (!QuestionBoxTextWidget) return;

	FText Text = FText::GetEmpty();

	if (SelectedRow->ItemType == EItemType::Miss)
	{
		// 꽝 → 이름만 출력
		Text = SelectedRow->ItemName;
	}
	else
	{
		// 아이템명 + 개수 출력
		Text = FText::Format(
			FText::FromString(TEXT("{0} {1}개 획득")),
			SelectedRow->ItemName,
			FText::AsNumber(SelectedRow->GenerateCount));
	}

	QuestionBoxTextWidget->SetText(Text);

	// 일정 시간 후 숨김
	StartHideWidgetTimer();
}

void UGA_QuestionBox::StartHideWidgetTimer() const
{
	if (!GetWorld() || !PC) return;

	if (PC)
	{
		FString PCName = PC->GetName(); // 보통 "PC_0", "PC_1" 이런 식
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Cyan,
				FString::Printf(TEXT("ActivateAbility 실행 주체: %s (%s)"),
					Character->HasAuthority() ? TEXT("Host(Server)") : TEXT("Guest(Client)"),
					*PCName)
			);
		}
	}

	
	PC->SetWidget(TEXT("QuestionBoxText"), true, EMessageTargetType::LocalClient);

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
		{
			PC->SetWidget(TEXT("QuestionBoxText"), false, EMessageTargetType::LocalClient);
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, QuestionBoxWidgetDuration, false);
}


