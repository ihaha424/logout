// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Objects/FRandomDT.h"
#include "UI/QuestionBoxTextWidget.h"
#include "GA_QuestionBox.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API UGA_QuestionBox : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_QuestionBox();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** 에디터에서 지정할 아이템 데이터테이블 */
	UPROPERTY(EditDefaultsOnly, Category="QuestionBox")
	UDataTable* ItemDataTable = nullptr;

private:
	/** 데이터테이블에서 뽑을 수 있는 행 수집 */
	void GatherPickableRows(TArray<FRandomDT*>& OutRows) const;

	/** 가중치(RandomProbability) 기반으로 랜덤 선택 */
	FRandomDT* SelectWeightedRandomRow(const TArray<FRandomDT*>& Rows) const;

	/** 선택된 아이템 처리 */
	bool ProcessSelectedRow(AActor* AvatarActor, FRandomDT* SelectedRow);

	/** 인벤토리에 아이템 추가 */
	void AddItemToInventory(EItemType ItemType, int32 Quantity);

	/** 위젯에 표시할 텍스트 설정 */
	void SetQuestionBoxWidget(FRandomDT* SelectedRow);

	/** 일정 시간 후 위젯 숨기기 타이머 실행 */
	void StartHideWidgetTimer() const;

private:
	TObjectPtr<class APlayerCharacter> Character = nullptr;
	TObjectPtr<class APS_Player> PS = nullptr;
	TObjectPtr<class APC_Player> PC = nullptr;

	/** 위젯 표시 유지 시간 */
	UPROPERTY(EditAnywhere, Category="QuestionBox | UI")
	float QuestionBoxWidgetDuration = 2.f;
};
