// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClearItemCounterWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UClearItemCounterWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    // 레벨 별 총 클리어 아이템 갯수 체크
    void SetMaxClearItem();

    // 클리어 아이템 표시용 함수
    void SetClearItemCount(int32 CurrentClearItem);


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TEXT_ClearItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClearItem")
    int32 MaxClearItem = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClearItem")
	TSubclassOf<class ADataFragment> ClearItemClass;

};
