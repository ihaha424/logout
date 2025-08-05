// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Objects/ItemData.h"
#include "ItemSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
    // 슬롯 초기화 - 아이콘과 수량 모두 비움
    UFUNCTION(BlueprintCallable)
    void ResetItemSlot();

    // 아이템 타입에 맞는 아이콘 설정
    UFUNCTION(BlueprintCallable)
    void SetItemIcon(EItemType eItemType);

    // 아이템 수량 설정
    UFUNCTION(BlueprintCallable)
    void SetItemQuantity(int32 ItemStack);

protected:
    // 에디터에서 할당 가능한 DataTable 참조
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UDataTable> ItemDataTable;

    // 바인딩된 아이콘 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> ItemIcon;

    // 바인딩된 텍스트 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> ItemQuantity;
};
