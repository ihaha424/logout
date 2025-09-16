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

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
    // 에디터에서 데이터테이블 지정
    UPROPERTY(EditDefaultsOnly, Category="QuestionBox")
    UDataTable* ItemDataTable;


private:
    // 데이터테이블에서 물음표박스에서 뽑을 수 있는 행들 수집
    void GatherPickableRows(TArray<FRandomDT*>& OutRows) const;

    // 가중치(RandomProbability)를 사용해 하나 선택
    FRandomDT* SelectWeightedRandomRow(const TArray<FRandomDT*>& Rows) const;

    // 선택된 행(아이템)을 처리: None(꽝)인지, 인벤토리에 넣을지 등
    bool ProcessSelectedRow(AActor* AvatarActor, FRandomDT* SelectedRow);

    // 인벤토리에 아이템을 실제로 추가하는 함수
	void AddItemToInventory(EItemType ItemType, int32 Quantity);


private:
	TObjectPtr<class APlayerCharacter> Character = nullptr;
	TObjectPtr<class APS_Player> PS = nullptr;

	// 데이터조각을 먹으면 팝업으로 뜰 위젯
	UPROPERTY(EditDefaultsOnly, Category = "QuestionBox | UI")
	TSubclassOf<UQuestionBoxTextWidget> QuestionBoxWidgetClass;

	UPROPERTY()
	TObjectPtr<UQuestionBoxTextWidget> QuestionBoxWidget;

	// 표시할 텍스트
	UPROPERTY(EditAnywhere, Category = "QuestionBox | UI")
	FText QuestionBoxText;

	// 위젯 표시 유지 시간
	UPROPERTY(EditAnywhere, Category = "QuestionBox | UI")
	int32 QuestionBoxWidgetDuration = 2;
};
