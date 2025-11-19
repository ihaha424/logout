#include "GA_QuestionBox.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Objects/InventoryComponent.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

UGA_QuestionBox::UGA_QuestionBox()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	FGameplayTagContainer DefaultTags;
	DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_UseRandomBox);
	SetAssetTags(DefaultTags);
}

void UGA_QuestionBox::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );

	PS = Character->GetPS();
	NULLCHECK_CODE_RETURN_LOG(PS, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );
	PC = Cast<APC_Player>(ActorInfo->PlayerController.Get());

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor || !ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_QuestionBox::ActivateAbility - Invalid AvatarActor or ItemDataTable"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	TArray<FRandomDT*> Rows;
	GatherPickableRows(Rows);
	
	FRandomDT* Selected = SelectWeightedRandomRow(Rows);

	if (PC && Selected)
	{
		//TPT_LOG(ObjectLog, Log, TEXT("Question Box Selected Item: %s, Quantity: %d"), *Selected->ItemName.ToString(), Selected->GenerateCount);
		SetQuestionBoxWidget(Selected);
		AddItemToInventory(Selected->ItemType, Selected->GenerateCount);
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

void UGA_QuestionBox::SetQuestionBoxWidget(FRandomDT* SelectedRow)
{
	if (!PC) return;

	FText Text;
	if (SelectedRow->ItemType == EItemType::Miss)
	{
		Text = SelectedRow->ItemName;
	}
	else
	{
		Text = FText::Format(
			FText::FromString(TEXT("{0} {1}개 획득")),
			SelectedRow->ItemName,
			FText::AsNumber(SelectedRow->GenerateCount));
	}
	// UI 표시 요청을 InventoryComponent에서 통합 수행
	PS->InventoryComp->ShowQuestionBoxResult(Text, QuestionBoxWidgetDuration);
}
