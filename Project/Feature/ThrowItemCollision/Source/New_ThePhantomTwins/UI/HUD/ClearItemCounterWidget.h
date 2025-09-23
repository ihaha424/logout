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
    // 클리어 아이템 표시용 함수
    void SetClearItemCount(int32 CurrentClearItem);


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TEXT_ClearItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataShard")
    int32 MaxClearItem = 5;

};
