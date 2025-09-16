// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Objects/FRandomDT.h"
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
    bool AddItemToInventory(AActor* AvatarActor, EItemType ItemType, int32 Quantity, const FRandomDT& RowData);
};
