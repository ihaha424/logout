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
	UPROPERTY(EditDefaultsOnly, Category="QuestionBox")
	UDataTable* ItemDataTable = nullptr;

private:
	void GatherPickableRows(TArray<FRandomDT*>& OutRows) const;
	FRandomDT* SelectWeightedRandomRow(const TArray<FRandomDT*>& Rows) const;
	void AddItemToInventory(EItemType ItemType, int32 Quantity);
	void SetQuestionBoxWidget(FRandomDT* SelectedRow);

private:
	TObjectPtr<class APlayerCharacter> Character = nullptr;
	TObjectPtr<class APS_Player> PS = nullptr;
	TObjectPtr<class APC_Player> PC = nullptr;
	UPROPERTY(EditAnywhere, Category="QuestionBox | UI")
	float QuestionBoxWidgetDuration = 2.0f;
};
