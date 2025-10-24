// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "Objects/ItemData.h"
#include "InventoryWidget.generated.h"

/**
 * 플레이어 인벤토리를 UI로 표시하는 위젯입니다.
 * WrapBox를 통해 슬롯 위젯들을 배치합니다.
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	/** 
	 * 인벤토리 데이터를 받아 슬롯들을 생성하고 WrapBox에 추가합니다.
	 * @param Inventory - 인벤토리 데이터 컴포넌트
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LoadInventory(class UInventoryComponent* Inventory);

	void LoadInventory(const int32 InventorySlotsNum);

	/**
	 * WrapBox의 자식 중 지정된 인덱스의 아이템 슬롯 위젯을 반환합니다.
	 * @param Index - 슬롯 인덱스
	 * @return 해당 인덱스의 UItemSlotWidget 포인터 (없으면 nullptr 반환)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class UItemSlotWidget* GetItemSlotWidget(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetToolTips(bool bVisible, EItemType eItemType);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemToolTipWidget> ItemToolTipWidget;

	/** 아이템 슬롯들을 배치하는 래핑 박스 위젯입니다. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWrapBox> WrapBox;

	/** 생성할 슬롯 위젯의 클래스입니다. BP_ItemSlotWidget 등을 에디터에서 지정하세요. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UItemSlotWidget> ItemSlotWidgetClass;
};