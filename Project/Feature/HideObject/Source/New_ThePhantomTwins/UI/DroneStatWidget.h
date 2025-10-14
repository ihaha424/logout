// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UDroneStatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    // 레벨 별 총 클리어 아이템 갯수 체크
    void SetMaxClearItem();

    // HP 체크
    void SetHP(int32 HP);

    // MP 체크
    void SetMP(int32 MP);

    // 클리어 아이템 표시용 함수
    void SetClearItemCount(int32 CurrentClearItem);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TEXT_ClearItem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TEXT_HP;

    UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TEXT_MP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClearItem")
    int32 MaxClearItem = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClearItem")
	TSubclassOf<class ADataFragment> ClearItemClass;
};
